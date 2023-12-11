/*************************************************************************
 * Copyright (C) 2023 Blue Brain Project
 *
 * This file is part of 'libsonata-mpi', distributed under the terms
 * of the GNU Lesser General Public License version 3.
 *
 * See top-level COPYING.LESSER and COPYING files for details.
 *************************************************************************/

#include <bbp/sonata/hdf5_collective_reader.h>

namespace bbp {
namespace sonata {

class Hdf5PluginCollectiveData
{
  public:
    Hdf5PluginCollectiveData() = default;
    Hdf5PluginCollectiveData(MPI_Comm comm, bool collective_metadata, bool collective_transfer)
        : comm(comm_dup(comm))
        , collective_metadata(collective_metadata)
        , collective_transfer(collective_transfer) {}

    ~Hdf5PluginCollectiveData() {
        MPI_Comm_free(&comm);
    }

    HighFive::FileAccessProps fapl() const {
        HighFive::FileAccessProps fapl;
        fapl.add(HighFive::MPIOFileAccess{MPI_COMM_WORLD, MPI_INFO_NULL});
        fapl.add(HighFive::MPIOCollectiveMetadata{collective_metadata});

        return fapl;
    }

    HighFive::DataTransferProps dxpl() const {
        HighFive::DataTransferProps dxpl;
        dxpl.add(HighFive::UseCollectiveIO{collective_transfer});

        return dxpl;
    }

  private:
    MPI_Comm comm_dup(MPI_Comm old_comm) {
        MPI_Comm new_comm;
        MPI_Comm_dup(old_comm, &new_comm);
        return new_comm;
    }

    MPI_Comm comm = MPI_COMM_SELF;
    bool collective_metadata = false;
    bool collective_transfer = false;
};


namespace detail {
template <class Range>
HighFive::HyperSlab make_hyperslab(const std::vector<Range>& ranges) {
    HighFive::HyperSlab slab;
    for (const auto& range : ranges) {
        size_t i_begin = std::get<0>(range);
        size_t i_end = std::get<1>(range);
        slab |= HighFive::RegularHyperSlab({i_begin}, {i_end - i_begin});
    }

    return slab;
}

HighFive::HyperSlab make_hyperslab(const Selection::Ranges& xranges,
                                   const Selection::Ranges& yranges) {
    if(yranges.size() != 1) {
        throw SonataError("Case `yranges.size() != 1` has not been implemented.");
    }
    const auto& yslice = yranges[0];

    HighFive::HyperSlab slab;

    size_t j_begin = std::get<0>(yslice);
    size_t j_end = std::get<1>(yslice);
    for (const auto& range : xranges) {
        size_t i_begin = std::get<0>(range);
        size_t i_end = std::get<1>(range);
        slab |= HighFive::RegularHyperSlab({i_begin, j_begin}, {i_end - i_begin, j_end - j_begin});
    }

    return slab;
}

}  // namespace detail

template <class T>
class Hdf5PluginRead1DCollective: virtual public Hdf5PluginCollectiveData,
                                virtual public Hdf5PluginRead1DInterface<T>
{
  public:
    std::vector<T> readSelection(const HighFive::DataSet& dset,
                                 const Selection& selection) const override {
        return dset.select(detail::make_hyperslab(selection.ranges())).template read<std::vector<T>>(this->dxpl());
    }
};

template <class T>
class Hdf5PluginRead2DCollective: virtual public Hdf5PluginCollectiveData,
                                virtual public Hdf5PluginRead2DInterface<T>
{
  private:
    using AltRanges = std::vector<std::array<uint64_t, 2>>;

  public:
    std::vector<T> readSelection(const HighFive::DataSet& dset,
                                 const Selection& xsel,
                                 const Selection& ysel) const override {
        auto dataspace = HighFive::DataSpace({xsel.flatSize(), 2});
        return dset.select(detail::make_hyperslab(xsel.ranges(), ysel.ranges()), dataspace)
            .template read<std::vector<T>>(this->dxpl());
    }
};

template <class T, class U>
class Hdf5PluginCollective;

template <class... Ts, class... Us>
class Hdf5PluginCollective<std::tuple<Ts...>, std::tuple<Us...>>
    : virtual public Hdf5PluginInterface<std::tuple<Ts...>, std::tuple<Us...>>,
      virtual public Hdf5PluginRead1DCollective<Ts>...,
      virtual public Hdf5PluginRead2DCollective<Us>...
{
  public:
    Hdf5PluginCollective(MPI_Comm comm, bool collective_metadata, bool collective_transfer)
        : Hdf5PluginCollectiveData(comm, collective_metadata, collective_transfer) {}

    HighFive::File openFile(const std::string& path) const override {
        return HighFive::File(path,
                              HighFive::File::ReadOnly,
                              static_cast<const Hdf5PluginCollectiveData&>(*this).fapl());
    }
};

Hdf5Reader make_collective_reader(MPI_Comm comm,
                                  bool collective_metadata,
                                  bool collective_transfer) {
    return Hdf5Reader(std::make_shared<Hdf5PluginCollective<Hdf5Reader::supported_1D_types, Hdf5Reader::supported_2D_types>>(
        comm, collective_metadata, collective_transfer));
}


}  // namespace sonata
}  // namespace bbp
