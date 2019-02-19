//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Database data structure (implementation)

   Database data structure containing e.g. name and enabled flag.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.12.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamProDatabaseData.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor of Database class.

   \created     05.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamProDatabaseData::C_CamProDatabaseData(void)
{
   this->c_Name = "DefaultDatabaseName";
   this->q_Enabled = true;
   this->s32_BusIndex = -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     05.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamProDatabaseData::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   // Database properties (name, state)
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.toStdString().c_str(), this->c_Name.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_Enabled, sizeof(this->q_Enabled), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->s32_BusIndex, sizeof(this->s32_BusIndex), oru32_HashValue);
}
