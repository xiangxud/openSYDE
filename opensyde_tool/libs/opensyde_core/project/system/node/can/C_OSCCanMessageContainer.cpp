//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Storage container for all CAN message types (implementation)

   Storage container for all CAN message types

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCCanMessageContainer.h"
#include "CSCLChecksums.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     06.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCCanMessageContainer::C_OSCCanMessageContainer(void) :
   q_IsComProtocolUsedByInterface(false)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     06.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCCanMessageContainer::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Tmp;

   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsComProtocolUsedByInterface,
                                      sizeof(this->q_IsComProtocolUsedByInterface), oru32_HashValue);

   for (uint32 u32_Counter = 0U; u32_Counter < this->c_RxMessages.size(); ++u32_Counter)
   {
      this->c_RxMessages[u32_Counter].CalcHash(oru32_HashValue);
   }

   for (uint32 u32_Counter = 0U; u32_Counter < this->c_TxMessages.size(); ++u32_Counter)
   {
      this->c_TxMessages[u32_Counter].CalcHash(oru32_HashValue);
   }

   //Force CRC change if single RX message is moved to empty TX (previously no change in CRC calculation)
   u32_Tmp = this->c_RxMessages.size();
   stw_scl::C_SCLChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
   u32_Tmp = this->c_TxMessages.size();
   stw_scl::C_SCLChecksums::CalcCRC32(&u32_Tmp, sizeof(u32_Tmp), oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Recalculate all data element indices

   Based on assumption data elements are sorted in data pool by message and signal

   \created     07.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCCanMessageContainer::ReCalcDataElementIndices(void)
{
   std::vector<C_OSCCanMessage> * pc_ComMessages;

   for (uint8 u8_Toggle = 0; u8_Toggle < 2; ++u8_Toggle)
   {
      uint32 u32_ListSignalIndex = 0;
      if ((u8_Toggle % 2) == 0)
      {
         pc_ComMessages = &this->c_TxMessages;
      }
      else
      {
         pc_ComMessages = &this->c_RxMessages;
      }
      //Check consistency
      for (uint32 u32_ItMessage = 0; u32_ItMessage < pc_ComMessages->size(); ++u32_ItMessage)
      {
         C_OSCCanMessage & rc_Message = (*pc_ComMessages)[u32_ItMessage];
         for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
         {
            C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
            rc_Signal.u32_ComDataElementIndex = u32_ListSignalIndex;
            ++u32_ListSignalIndex;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get start of message in data pool list element vector

   \param[in] orq_IsTx           Flag if message is tx (else rx)
   \param[in] oru32_MessageIndex Message index

   \return
   Data pool list element index of message start

   \created     10.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_OSCCanMessageContainer::GetMessageSignalDataStartIndex(const bool & orq_IsTx,
                                                                const uint32 & oru32_MessageIndex) const
{
   uint32 u32_Retval = 0;

   const std::vector<C_OSCCanMessage> & rc_ComMessages = this->GetMessagesConst(orq_IsTx);

   //Check consistency
   for (uint32 u32_ItMessage = 0; (u32_ItMessage < rc_ComMessages.size()) && (u32_ItMessage < oru32_MessageIndex);
        ++u32_ItMessage)
   {
      const C_OSCCanMessage & rc_Message = rc_ComMessages[u32_ItMessage];
      for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
      {
         ++u32_Retval;
      }
   }

   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request reference to either list

   \param[in] orq_IsTx Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)

   \created     10.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const std::vector<C_OSCCanMessage> & C_OSCCanMessageContainer::GetMessagesConst(const bool & orq_IsTx) const
{
   if (orq_IsTx == true)
   {
      return this->c_TxMessages;
   }
   else
   {
      return this->c_RxMessages;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Request reference to either list

   \param[in] orq_IsTx Flag if tx message vector was requested (else rx message vector is output)

   \return
   Either tx or rx list (as requested)

   \created     10.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<C_OSCCanMessage> & C_OSCCanMessageContainer::GetMessages(const bool & orq_IsTx)
{
   if (orq_IsTx == true)
   {
      return this->c_TxMessages;
   }
   else
   {
      return this->c_RxMessages;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check message error

   IMPORTANT: some error types (e.g. name & id conflict) can only be checked via system definition

   \param[in]  opc_List                             Node data pool list containing signal data
                                                    (Optional as it is only required by some checks)
   \param[in]  oru32_MessageIndex                   Message index
   \param[in]  orq_IsTx                             Flag if message is tx (else rx)
   \param[out] opq_NameConflict                     Name conflict
   \param[out] opq_NameInvalid                      Name not usable as variable
   \param[out] opq_DelayTimeInvalid                 Delay time is invalid
   \param[out] opq_IdConflict                       Id conflict
   \param[out] opq_IdInvalid                        Id out of 11 bit / 29 bit range
   \param[out] opq_SignalInvalid                    An error found for a signal
   \param[in]  ou32_CANMessageValidSignalsDLCOffset CAN message DLC offset for valid signal range check

   \created     10.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OSCCanMessageContainer::CheckMessageLocalError(const C_OSCNodeDataPoolList * const opc_List,
                                                      const uint32 & oru32_MessageIndex, const bool & orq_IsTx,
                                                      bool * const opq_NameConflict, bool * const opq_NameInvalid,
                                                      bool * const opq_DelayTimeInvalid, bool * const opq_IdConflict,
                                                      bool * const opq_IdInvalid, bool * const opq_SignalInvalid,
                                                      const uint32 ou32_CANMessageValidSignalsDLCOffset) const
{
   const std::vector<C_OSCCanMessage> & rc_Messages = this->GetMessagesConst(orq_IsTx);

   if (oru32_MessageIndex < rc_Messages.size())
   {
      const C_OSCCanMessage & rc_Message = rc_Messages[oru32_MessageIndex];

      //Check variable name
      if (opq_NameInvalid != NULL)
      {
         if (C_OSCUtils::h_CheckValidCName(rc_Message.c_Name) == false)
         {
            *opq_NameInvalid = true;
         }
         else
         {
            *opq_NameInvalid = false;
         }
      }
      //Name conflict
      if (opq_NameConflict != NULL)
      {
         *opq_NameConflict = false;
         //Tx
         for (uint32 u32_ItMessage = 0; (u32_ItMessage < this->c_TxMessages.size()) && (*opq_NameConflict == false);
              ++u32_ItMessage)
         {
            bool q_Skip = false;

            if (orq_IsTx == true)
            {
               if (u32_ItMessage == oru32_MessageIndex)
               {
                  q_Skip = true;
               }
            }
            if (q_Skip == false)
            {
               const C_OSCCanMessage & rc_CurrentMessage = this->c_TxMessages[u32_ItMessage];
               if (rc_Message.c_Name.LowerCase() == rc_CurrentMessage.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
         //Rx
         for (uint32 u32_ItMessage = 0; (u32_ItMessage < this->c_RxMessages.size()) && (*opq_NameConflict == false);
              ++u32_ItMessage)
         {
            bool q_Skip = false;

            if (orq_IsTx == false)
            {
               if (u32_ItMessage == oru32_MessageIndex)
               {
                  q_Skip = true;
               }
            }
            if (q_Skip == false)
            {
               const C_OSCCanMessage & rc_CurrentMessage = this->c_RxMessages[u32_ItMessage];
               if (rc_Message.c_Name.LowerCase() == rc_CurrentMessage.c_Name.LowerCase())
               {
                  *opq_NameConflict = true;
               }
            }
         }
      }

      //Id
      //Check valid id
      if (opq_IdInvalid != NULL)
      {
         if (rc_Message.q_IsExtended == true)
         {
            if (rc_Message.u32_CanId <= 0x1FFFFFFFUL)
            {
               *opq_IdInvalid = false;
            }
            else
            {
               *opq_IdInvalid = true;
            }
         }
         else
         {
            if (rc_Message.u32_CanId <= 0x7FFUL)
            {
               *opq_IdInvalid = false;
            }
            else
            {
               *opq_IdInvalid = true;
            }
         }
      }
      //Name conflict
      if (opq_IdConflict != NULL)
      {
         *opq_IdConflict = false;
         //Tx
         for (uint32 u32_ItMessage = 0; (u32_ItMessage < this->c_TxMessages.size()) && (*opq_IdConflict == false);
              ++u32_ItMessage)
         {
            bool q_Skip = false;

            if (orq_IsTx == true)
            {
               if (u32_ItMessage == oru32_MessageIndex)
               {
                  q_Skip = true;
               }
            }
            if (q_Skip == false)
            {
               const C_OSCCanMessage & rc_CurrentMessage = this->c_TxMessages[u32_ItMessage];
               if (rc_Message.u32_CanId == rc_CurrentMessage.u32_CanId)
               {
                  *opq_IdConflict = true;
               }
            }
         }
         //Rx
         for (uint32 u32_ItMessage = 0; (u32_ItMessage < this->c_RxMessages.size()) && (*opq_IdConflict == false);
              ++u32_ItMessage)
         {
            bool q_Skip = false;

            if (orq_IsTx == false)
            {
               if (u32_ItMessage == oru32_MessageIndex)
               {
                  q_Skip = true;
               }
            }
            if (q_Skip == false)
            {
               const C_OSCCanMessage & rc_CurrentMessage = this->c_RxMessages[u32_ItMessage];
               if (rc_Message.u32_CanId == rc_CurrentMessage.u32_CanId)
               {
                  *opq_IdConflict = true;
               }
            }
         }
      }
      //Check
      if (opq_DelayTimeInvalid != NULL)
      {
         *opq_DelayTimeInvalid = false;
         if (rc_Message.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)
         {
            if (static_cast<uint32>(rc_Message.u16_DelayTimeMs) > rc_Message.u32_CycleTimeMs)
            {
               *opq_DelayTimeInvalid = true;
            }
         }
      }

      //Signals
      if (opq_SignalInvalid != NULL)
      {
         *opq_SignalInvalid = false;
         for (uint32 u32_ItSignal = 0; (u32_ItSignal < rc_Message.c_Signals.size()) && (*opq_SignalInvalid == false);
              ++u32_ItSignal)
         {
            if (rc_Message.CheckErrorSignal(opc_List, u32_ItSignal, ou32_CANMessageValidSignalsDLCOffset))
            {
               *opq_SignalInvalid = true;
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check messages error

   IMPORTANT: some error types (e.g. name & id conflict) can only be checked via system definition

   \param[in]     orc_ListTx                           Node data pool list containing tx signal data
   \param[in]     orc_ListRx                           Node data pool list containing rx signal data
   \param[in]     ou32_CANMessageValidSignalsDLCOffset CAN message DLC offset for valid signal range check
   \param[in,out] opc_InvalidTxMessages                Optional vector of invalid TX CAN message names
   \param[in,out] opc_InvalidRxMessages                Optional vector of invalid RX CAN message names

   \return
   true  Error
   false No error detected

   \created     10.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCCanMessageContainer::CheckLocalError(const C_OSCNodeDataPoolList & orc_ListTx,
                                               const C_OSCNodeDataPoolList & orc_ListRx,
                                               const uint32 ou32_CANMessageValidSignalsDLCOffset,
                                               std::vector<uint32> * const opc_InvalidTxMessages,
                                               std::vector<uint32> * const opc_InvalidRxMessages) const
{
   bool q_Error = false;

   for (uint32 u32_ItMessage = 0;
        (u32_ItMessage < this->c_TxMessages.size()) && ((q_Error == false) || (opc_InvalidTxMessages != NULL));
        ++u32_ItMessage)
   {
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_DelayInvalid = false;
      bool q_IdConflict = false;
      bool q_IdInvalid = false;
      bool q_SignalInvalid = false;
      this->CheckMessageLocalError(&orc_ListTx, u32_ItMessage, true, &q_NameConflict, &q_NameInvalid, &q_DelayInvalid,
                                   &q_IdConflict, &q_IdInvalid, &q_SignalInvalid, ou32_CANMessageValidSignalsDLCOffset);
      if ((((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_DelayInvalid == true)) ||
            (q_IdConflict == true)) || (q_IdInvalid == true)) || (q_SignalInvalid == true))
      {
         q_Error = true;
         if (opc_InvalidTxMessages != NULL)
         {
            opc_InvalidTxMessages->push_back(u32_ItMessage);
         }
      }
   }
   for (uint32 u32_ItMessage = 0;
        (u32_ItMessage < this->c_RxMessages.size()) && ((q_Error == false) || (opc_InvalidRxMessages != NULL));
        ++u32_ItMessage)
   {
      bool q_NameConflict = false;
      bool q_NameInvalid = false;
      bool q_DelayInvalid = false;
      bool q_IdConflict = false;
      bool q_IdInvalid = false;
      bool q_SignalInvalid = false;
      this->CheckMessageLocalError(&orc_ListRx, u32_ItMessage, false, &q_NameConflict, &q_NameInvalid, &q_DelayInvalid,
                                   &q_IdConflict, &q_IdInvalid, &q_SignalInvalid, ou32_CANMessageValidSignalsDLCOffset);
      if ((((((q_NameConflict == true) || (q_NameInvalid == true)) || (q_DelayInvalid == true)) ||
            (q_IdConflict == true)) || (q_IdInvalid == true)) || (q_SignalInvalid == true))
      {
         q_Error = true;
         if (opc_InvalidRxMessages != NULL)
         {
            opc_InvalidRxMessages->push_back(u32_ItMessage);
         }
      }
   }
   return q_Error;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check whether container contains at least one TX or RX message

   \return
   true   at least one TX or RX message is defined
   false  else

   \created     28.11.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool C_OSCCanMessageContainer::ContainsAtLeastOneMessage(void) const
{
   bool q_Result = false;

   if ((this->c_TxMessages.size() > 0) || (this->c_RxMessages.size() > 0))
   {
      q_Result = true;
   }
   return q_Result;
}
