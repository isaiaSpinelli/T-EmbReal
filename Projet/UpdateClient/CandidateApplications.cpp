#include "CandidateApplications.h"
#include "FlashUpdater.h"
#include "UCErrorCodes.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#include "mbed_trace.h"
#define TRACE_GROUP "CandidateApplications"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace update_client {

CandidateApplications::CandidateApplications(FlashUpdater& flashUpdater, uint32_t storageAddress, uint32_t storageSize, uint32_t headerSize, uint32_t nbrOfSlots) :
  m_flashUpdater(flashUpdater),
  m_storageAddress(storageAddress),
  m_storageSize(storageSize),
  m_nbrOfSlots(nbrOfSlots) {
  // the number of slots must be equal or smaller than MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS
  if (nbrOfSlots <= MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS) {  
    for (uint32_t slotIndex = 0; slotIndex < nbrOfSlots; slotIndex++) {
      uint32_t applicationAddress = 0;
      uint32_t slotSize = 0;
      int32_t result = getApplicationAddress(slotIndex, applicationAddress, slotSize);
      if (result != UC_ERR_NONE) {
        tr_error(" Application at slot %d is not valid: %d", slotIndex, result);
        continue;
      } 

      tr_debug(" Slot %d: application header address: 0x%08x application address 0x%08x (slot size %d)", 
               slotIndex, applicationAddress, applicationAddress + headerSize, slotSize);
      m_candidateApplicationArray[slotIndex] = new update_client::MbedApplication(m_flashUpdater, applicationAddress, applicationAddress + headerSize);
    }
  }
}

CandidateApplications::~CandidateApplications() {
  for (uint32_t slotIndex = 0; slotIndex < m_nbrOfSlots; slotIndex++) {
    delete m_candidateApplicationArray[slotIndex];
    m_candidateApplicationArray[slotIndex] = NULL;
  }
}

MbedApplication& CandidateApplications::getMbedApplication(uint32_t slotIndex) {
  return *m_candidateApplicationArray[slotIndex];
}


uint32_t CandidateApplications::getSlotForCandidate() { 
    update_client::MbedApplication* olderApp = NULL;
    uint32_t olderSlotIndex = 0;
    
    for (uint32_t slotIndex = 0; slotIndex < m_nbrOfSlots; slotIndex++) {

        // save the older app
        if (olderApp == NULL || olderApp->isNewerThan(*m_candidateApplicationArray[slotIndex]) ){
            olderApp = m_candidateApplicationArray[slotIndex];
            olderSlotIndex = slotIndex;
        }

        // if a slot contains a not valid app
       if ( !m_candidateApplicationArray[slotIndex]->isValid() ){
           return slotIndex;
       }

    }

    // all slot contains a valid app
    // so return index of older app
    return olderSlotIndex;
}

int32_t CandidateApplications::getApplicationAddress(uint32_t slotIndex, uint32_t& applicationAddress, uint32_t& slotSize) const {
   tr_debug(" Slot %d: Storage address: 0x%08x Storage size: %d", slotIndex, m_storageAddress, m_storageSize);

   // find the start address of the whole storage area. It needs to be aligned to
   // sector boundary and we cannot go outside user defined storage area, hence
   // rounding up to sector boundary
   uint32_t storageStartAddr = m_flashUpdater.alignAddressToSector(m_storageAddress, false);
   tr_debug(" Storage start address (slot %d): 0x%08x", slotIndex, storageStartAddr);

   // find the end address of the whole storage area. It needs to be aligned to
   // sector boundary and we cannot go outside user defined storage area, hence
   // rounding down to sector boundary 
   uint32_t storageEndAddr = m_flashUpdater.alignAddressToSector(m_storageAddress + m_storageSize, true);
   tr_debug(" Storage end address (slot %d): 0x%08x", slotIndex, storageEndAddr);
   
   // find the maximum size each slot can have given the start and end, without
   // considering the alignment of individual slots
   uint32_t maxSlotSize = (storageEndAddr - storageStartAddr) / m_nbrOfSlots;
   tr_debug(" maxSlotSize (slot %d): %d", slotIndex, maxSlotSize);

   // find the start address of slot. It needs to align to sector boundary. We
   // choose here to round down at each slot boundary 
   uint32_t slotStartAddr = m_flashUpdater.alignAddressToSector(storageStartAddr + slotIndex * maxSlotSize, true);
   tr_debug(" Slot start address (slot %d): 0x%08x", slotIndex, slotStartAddr);
   
   // find the end address of the slot, rounding down to sector boundary same as
   // the slot start address so that we make sure two slot don't overlap 
   uint32_t slotEndAddr = m_flashUpdater.alignAddressToSector(slotStartAddr + maxSlotSize, true);
   tr_debug(" Slot end address (slot %d): 0x%08x", slotIndex, slotEndAddr);
   
   applicationAddress = slotStartAddr;
   slotSize = slotEndAddr - slotStartAddr;
   
   return UC_ERR_NONE;
}

bool CandidateApplications::hasValidNewerApplication(MbedApplication& activeApplication, uint32_t& newestSlotIndex) const {
  tr_debug(" Checking for newer applications on %d slots", m_nbrOfSlots);
  newestSlotIndex = m_nbrOfSlots;
  for (uint32_t slotIndex = 0; slotIndex < m_nbrOfSlots; slotIndex++) {    
    // Only hash check firmwares with higher version number than the
    // active image and with a different hash. This prevents rollbacks
    // and hash checks of old images. If the active image is not valid,
    // bestStoredFirmwareImageDetails.version equals 0
    tr_debug(" Checking application at slot %d", slotIndex);
    MbedApplication& newestApplication = newestSlotIndex == m_nbrOfSlots ? activeApplication : *m_candidateApplicationArray[newestSlotIndex];
    if (m_candidateApplicationArray[slotIndex]->isNewerThan(newestApplication)) {
#if MBED_CONF_MBED_TRACE_ENABLE
      if (newestSlotIndex == m_nbrOfSlots)
        tr_debug(" Candidate application at slot %d is newer than the active one", slotIndex);
      else
        tr_debug(" Candidate application at slot %d is newer than application at slot %d", slotIndex, newestSlotIndex);
#endif      
      int32_t result = m_candidateApplicationArray[slotIndex]->checkApplication();
      if (result != UC_ERR_NONE) {
        tr_error(" Candidate application at slot %d is not valid: %d", slotIndex, result);
        continue;
      }
      tr_debug(" Candidate application at slot %d is valid", slotIndex);

      // update the newest slot index
      newestSlotIndex = slotIndex;
    }
  }
  return newestSlotIndex != m_nbrOfSlots;
}

#if defined(POST_APPLICATION_ADDR)
int32_t CandidateApplications::installApplication(uint32_t slotIndex, uint32_t destHeaderAddress) {  
  tr_debug(" Installing candidate application at slot %d as active application", slotIndex);
  const uint32_t pageSize = m_flashUpdater.get_page_size();
  tr_debug("Flash page size is %d\r\n", pageSize);

  std::unique_ptr<char> writePageBuffer = std::unique_ptr<char>(new char[pageSize]);
  std::unique_ptr<char> readPageBuffer = std::unique_ptr<char>(new char[pageSize]);

  uint32_t destAddr = destHeaderAddress;
  uint32_t sourceAddr = 0;
  uint32_t slotSize = 0;
  int32_t result = getApplicationAddress(slotIndex, sourceAddr, slotSize);
  if (result != UC_ERR_NONE) {
    tr_error("Cannot get address of candidate application at slot %d\r\n", slotIndex);
    return result;
  }

  const uint32_t destSectorSize = m_flashUpdater.get_sector_size(destAddr);      
  uint32_t nextDestSectorAddress = destAddr + destSectorSize;
  bool destSectorErased = false;
  size_t destPagesFlashed = 0;  
  
  // add the header size to the firmware size
  const uint32_t headerSize = POST_APPLICATION_ADDR - HEADER_ADDR;
  tr_debug(" Header size is %d\r\n", headerSize);  
  const uint64_t copySize = m_candidateApplicationArray[slotIndex]->getFirmwareSize() + headerSize;

  uint32_t nbrOfBytes = 0;
  tr_debug(" Starting to copy application from address 0x%08x to address 0x%08x\r\n", sourceAddr, destAddr);
  
  while (nbrOfBytes < copySize) {
    // TODO: read a page from the candidate location and write it to the active application

    // ---- READ A PAGE FROM CANDIDATE LOCATION (to writePageBuffer)-----
    uint32_t addr_1 = sourceAddr+nbrOfBytes;
    result = m_flashUpdater.readPage(pageSize, writePageBuffer.get(), addr_1  );

     if (result != UC_ERR_NONE) {
        tr_error("Cannot read candidate application (address 0x%08x)\r\n", addr_1);
        break;       
    }


    // ---- WRITE THE READ PAGE TO ACTIVE APP (from writePageBuffer)-----
    uint32_t addr_2 = destAddr  + nbrOfBytes;
    //bool sectorErased = true;
    //size_t pagesFlashed = 0;
    //uint32_t nextSectorAddress;
    result = m_flashUpdater.writePage(pageSize, writePageBuffer.get(), readPageBuffer.get(), 
                                    addr_2, destSectorErased , destPagesFlashed, nextDestSectorAddress);
    if (result != UC_ERR_NONE) {
        tr_error("Cannot write candidate application to active app\r\n");
        break;       
    }

    // update progress
    nbrOfBytes += pageSize;    
#if MBED_CONF_MBED_TRACE_ENABLE
    // tr_debug("Copied %05d bytes", nbrOfBytes);
#endif
  }
  tr_debug(" Copied %d bytes", nbrOfBytes);
  writePageBuffer = NULL;
  readPageBuffer = NULL;

  return UC_ERR_NONE;
}
#endif







bool CandidateApplications::hasValidCandidate(uint32_t& slotIndexValid) const {

  for (uint32_t slotIndex = 0; slotIndex < m_nbrOfSlots; slotIndex++) {    

    if (m_candidateApplicationArray[slotIndex]->isValid()) {
        slotIndexValid =  slotIndex;
        return true;
    }

  }
  return false;
}

} // namespace
