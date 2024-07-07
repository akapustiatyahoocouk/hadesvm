//
//  make-bootable-action.cpp - "make bootable" action
//
//////////
#include "main.hpp"

namespace
{
    #include "data/boot-sector-ibmpc-hades.bin.hxx"
    #include "data/boot-sector-cereon-workstation-hades-be.bin.hxx"
    #include "data/boot-sector-cereon-workstation-hades-le.bin.hxx"
}

const char * MakeBootableAction::_vfdBootLoaderFileName = "/bootload";
const char * MakeBootableAction::_vfdLoadMapFileName = "/loadmap";

//////////
//  Action
bool MakeBootableAction::execute(Vfd *& currentVfd)
{
    assert(currentVfd != NULL);

    Fat12FileSystem fileSystem;
    fileSystem.bind(currentVfd);
    fileSystem.loadVfd();

    printf("Making disk bootable with %s\n", _bootLoaderImagePath.c_str());

    //  Prepare the copy of the boot sector image - we'll patch it with the load table
    uint8_t bootSectorImage[512];
    switch (_platform)
    {
        case Platform::IbmPc:
            assert(_________src_qt_vfd_utils_data_boot_sector_ibmpc_hades_bin_len == 512);
            memcpy(bootSectorImage, _________src_qt_vfd_utils_data_boot_sector_ibmpc_hades_bin, 512);
            break;
        case Platform::CereonWorkstationBigEndian:
            assert(_________src_qt_vfd_utils_data_boot_sector_cereon_workstation_hades_be_bin_len == 512);
            memcpy(bootSectorImage, _________src_qt_vfd_utils_data_boot_sector_cereon_workstation_hades_be_bin, 512);
            break;
        case Platform::CereonWorkstationLittleEndian:
            assert(_________src_qt_vfd_utils_data_boot_sector_cereon_workstation_hades_le_bin_len == 512);
            memcpy(bootSectorImage, _________src_qt_vfd_utils_data_boot_sector_cereon_workstation_hades_le_bin, 512);
            break;
        default:
            assert(false);
    }

    //  Load the boot loader image
    FILE * f = fopen(_bootLoaderImagePath.c_str(), "rb");
    if (f == NULL)
    {   //  OOPS!
        printf("*** ERROR: Boot loader file '%s' does not exist\n", _bootLoaderImagePath.c_str());
        return false;
    }
    fseek(f, 0, SEEK_END);
    size_t bootLoaderImageSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t * bootLoaderImage =  new uint8_t[bootLoaderImageSize];
    if (fread(bootLoaderImage, 1, bootLoaderImageSize, f) != bootLoaderImageSize)
    {
        printf("*** ERROR: Boot sector loaded '%s' is corrupt\n", _bootLoaderImagePath.c_str());
        delete [] bootLoaderImage;
        fclose(f);
        return false;
    }
    fclose(f);

    //  Create the boot loader file
    printf("Copying boot loader %s to VFD file %s\n", _bootLoaderImagePath.c_str(), _vfdBootLoaderFileName);
    FileChannel * bootLoaderChannel = fileSystem.createFile(_vfdBootLoaderFileName);
    if (bootLoaderChannel == NULL)
    {
        printf("*** ERROR: Failed to create VFD file '%s'\n", _vfdBootLoaderFileName);
        delete [] bootLoaderImage;
        return false;
    }
    if (bootLoaderChannel->write(bootLoaderImage, bootLoaderImageSize) != bootLoaderImageSize)
    {
        printf("*** ERROR: Failed to write VFD file '%s'\n", _vfdBootLoaderFileName);
        delete [] bootLoaderImage;
        return false;
    }
    delete bootLoaderChannel;
    delete [] bootLoaderImage;

    //  Now we need to prepare the load map - and for this we need to know the clusters
    //  which are occupied by the bootloader
    vector<uint16_t> bootLoaderClusterNumbers;
    fileSystem.getFileClusters(_vfdBootLoaderFileName, bootLoaderClusterNumbers);
    vector<uint16_t> bootLoaderLbaSectors;
    for (size_t i = 0; i < bootLoaderClusterNumbers.size(); i++)
    {
        bootLoaderLbaSectors.push_back(fileSystem.getLbaSectorForClusterNumber(bootLoaderClusterNumbers[i]));
    }

    //  Load map entries can load up to 32 sectors each, but they cannot span multiple tracks
    //  Note that FAT cluster 0 is LBA sector e.g. 33
    size_t sectorsPerTrack = currentVfd->getGeometry().getSectors();
    uint32_t loadAddress, bootLoaderEntryPointAddress;
    switch (_platform)
    {
        case Platform::IbmPc:
            loadAddress = bootLoaderEntryPointAddress = 0x00008000;
            break;
        case Platform::CereonWorkstationBigEndian:
        case Platform::CereonWorkstationLittleEndian:
            loadAddress = bootLoaderEntryPointAddress = 0x00030000;
            break;
        default:
            loadAddress = 0;    //  ...to shut up the compiler
            assert(false);
    }

    vector<_LoadMapEntry> loadMapEntries;
    for (size_t startClusterIndex = 0; startClusterIndex < bootLoaderLbaSectors.size(); )
    {   //  Determine the end cluster index for this load chunk - it must be <= 32 sectors
        //  away from the start chunk, it must be on the same track+head, all sectors from
        //  start to end must be continuous and the loaded address range must not cross
        //  the 64K DMA boundary
        size_t endClusterIndex;
        for (endClusterIndex = startClusterIndex + 1;
             endClusterIndex < bootLoaderLbaSectors.size() &&
                 endClusterIndex / sectorsPerTrack == startClusterIndex / sectorsPerTrack &&
                 endClusterIndex < startClusterIndex + 32 &&
                 bootLoaderLbaSectors[endClusterIndex] == bootLoaderLbaSectors[endClusterIndex - 1] + 1 &&
                 (loadAddress / 65536) == ((loadAddress + 512 * (endClusterIndex - startClusterIndex)) / 65536);
             endClusterIndex++)
        {   //  All work done in the loop header
        }
        //  Create load map entry
        assert((loadAddress & 0x0F) == 0);
        uint16_t pageToLoadAt;
        switch (_platform)
        {
            case Platform::IbmPc:
                pageToLoadAt = static_cast<uint16_t>(loadAddress >> 4);
                break;
            case Platform::CereonWorkstationBigEndian:
            case Platform::CereonWorkstationLittleEndian:
                pageToLoadAt = static_cast<uint16_t>(loadAddress >> 9);
                break;
            default:
                pageToLoadAt = 0;   //  ...to shut up the compiler
                assert(false);
        }
        _LoadMapEntry loadMapEntry(static_cast<unsigned>(endClusterIndex - startClusterIndex),
                                   bootLoaderLbaSectors[startClusterIndex],
                                   pageToLoadAt);
        loadMapEntries.push_back(loadMapEntry);
        //  Advance past this chunk
        loadAddress += static_cast<uint32_t>((endClusterIndex - startClusterIndex) * 512);
        startClusterIndex = endClusterIndex;
    }
    assert(loadMapEntries.size() < 64); //  TODO for now

    //  Create the load map file
    FileChannel * loadMapChannel = fileSystem.createFile(_vfdLoadMapFileName);
    if (loadMapChannel == NULL)
    {
        printf("*** ERROR: Failed to create VFD file '%s'\n", _vfdLoadMapFileName);
        return false;
    }

    //  Write load map entries
    for (size_t i = 0; i < loadMapEntries.size(); i++)
    {
        uint8_t loadMapEntryBytes[8];
        switch (_platform)
        {
            case Platform::IbmPc:
                loadMapEntryBytes[0] = static_cast<uint8_t>(0x40 | (loadMapEntries[i]._sectorsToLoad - 1));
                loadMapEntryBytes[1] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt);
                loadMapEntryBytes[2] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt >> 8);
                loadMapEntryBytes[3] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector);
                loadMapEntryBytes[4] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 8);
                loadMapEntryBytes[5] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 16);
                loadMapEntryBytes[6] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 24);
                loadMapEntryBytes[7] = 0;
                break;
            case Platform::CereonWorkstationBigEndian:
                loadMapEntryBytes[0] = static_cast<uint8_t>(0x40 | (loadMapEntries[i]._sectorsToLoad - 1));
                loadMapEntryBytes[1] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt >> 8);
                loadMapEntryBytes[2] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt);
                loadMapEntryBytes[3] = 0;
                loadMapEntryBytes[4] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 24);
                loadMapEntryBytes[5] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 16);
                loadMapEntryBytes[6] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 8);
                loadMapEntryBytes[7] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 0);
                break;
            case Platform::CereonWorkstationLittleEndian:
                loadMapEntryBytes[7] = static_cast<uint8_t>(0x40 | (loadMapEntries[i]._sectorsToLoad - 1));
                loadMapEntryBytes[6] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt >> 8);
                loadMapEntryBytes[5] = static_cast<uint8_t>(loadMapEntries[i]._pageToLoadAt);
                loadMapEntryBytes[4] = 0;
                loadMapEntryBytes[3] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 24);
                loadMapEntryBytes[2] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 16);
                loadMapEntryBytes[1] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 8);
                loadMapEntryBytes[0] = static_cast<uint8_t>(loadMapEntries[i]._firstLbaSector >> 0);
                break;
            default:
                assert(false);
        }
        if (loadMapChannel->write(loadMapEntryBytes, 8) != 8)
        {
            printf("*** ERROR: Failed to write VFD file '%s'\n", _vfdLoadMapFileName);
            return false;
        }
    }

    //  Write load map terminator
    const uint8_t * terminator;
    static const uint8_t IbmPcTerminator[8] = { 0x00, 0x00, 0x00, 0xEA, 0x00, 0x80, 0x00, 0x00 };
    static const uint8_t CereonWorkstationBigEndianTerminator[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00 };
    static const uint8_t CereonWorkstationLittleEndianTerminator[8] = { 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 };
    switch (_platform)
    {
        case Platform::IbmPc:
            terminator = IbmPcTerminator;
            break;
        case Platform::CereonWorkstationBigEndian:
            terminator = CereonWorkstationBigEndianTerminator;
            break;
        case Platform::CereonWorkstationLittleEndian:
            terminator = CereonWorkstationLittleEndianTerminator;
            break;
        default:
            terminator = CereonWorkstationLittleEndianTerminator;   //  ...to shut up the compiler
            assert(false);
    }
    if (loadMapChannel->write(terminator, 8) != 8)
    {
        printf("*** ERROR: Failed to write VFD file '%s'\n", _vfdLoadMapFileName);
        return false;
    }

    delete loadMapChannel;

    //  The boot loader and boot table files must both be marked as system
    fileSystem.setFileAttributes(_vfdBootLoaderFileName,
                                 Fat12FileSystem::FileAttribute_System | Fat12FileSystem::FileAttribute_ReadOnly);
    fileSystem.setFileAttributes(_vfdLoadMapFileName,
                                 Fat12FileSystem::FileAttribute_System | Fat12FileSystem::FileAttribute_ReadOnly);

    //  We need the LBA sector number where the load map starts
    vector<uint16_t> loadMapClusterNumbers;
    fileSystem.getFileClusters(_vfdLoadMapFileName, loadMapClusterNumbers);
    assert(loadMapClusterNumbers.size() == 1);

    //  Insert the load map cluster infos into the boot sector.
    //  Note that FAT cluster 0 is LBA sector e.g. 33
    uint64_t * loadTable = reinterpret_cast<uint64_t*>(bootSectorImage + 0x1D0);
    switch (_platform)
    {
        case Platform::IbmPc:
            loadTable[0] = fileSystem.getLbaSectorForClusterNumber(loadMapClusterNumbers[0]);
            break;
        case Platform::CereonWorkstationBigEndian:
#if defined _MSC_VER
            loadTable[0] = _byteswap_uint64(fileSystem.getLbaSectorForClusterNumber(loadMapClusterNumbers[0]));
#else
            loadTable[0] = std::byteswap<uint64_t>(fileSystem.getLbaSectorForClusterNumber(loadMapClusterNumbers[0]));
#endif
            break;
        case Platform::CereonWorkstationLittleEndian:
            loadTable[0] = fileSystem.getLbaSectorForClusterNumber(loadMapClusterNumbers[0]);
            break;
        default:
            assert(false);
    }
    loadTable[1] = loadTable[2] = loadTable[3] = ~(static_cast<uint64_t>(0));
    currentVfd->writeData(0, 0, bootSectorImage, 512);

    //  Cleanup & we're done
    return true;    //  TODO always ?
}

//  End of make-bootable-action.cpp
