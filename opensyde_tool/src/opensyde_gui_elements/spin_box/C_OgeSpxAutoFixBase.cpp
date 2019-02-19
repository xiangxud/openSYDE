//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Spin box with auto fix to nearest raw value base functionaliy (implementation)

   Spin box with auto fix to nearest raw value base functionaliy

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OgeSpxAutoFixBase.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     13.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxAutoFixBase::C_OgeSpxAutoFixBase(void) :
   mu64_NumberOfStepsAvailable(0),
   mf64_Factor(1.0),
   mf64_Offset(0.0),
   mu32_Index(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     21.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpxAutoFixBase::~C_OgeSpxAutoFixBase(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize widget with valid values

   \param[in] orc_Min     Unscaled minimum value
   \param[in] orc_Max     Unscaled maximum value
   \param[in] of64_Factor Scaling factor
   \param[in] of64_Offset Scaling offset
   \param[in] ou32_Index  Optional data index (used if array)

   \created     13.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpxAutoFixBase::Init(const C_OSCNodeDataPoolContent & orc_Min, const C_OSCNodeDataPoolContent & orc_Max,
                               const float64 of64_Factor, const float64 of64_Offset, const uint32 ou32_Index)
{
   this->mc_UnscaledMin = orc_Min;
   this->mc_UnscaledMax = orc_Max;
   if (C_SdNdeDataPoolContentUtil::h_GetNumberOfAvailableSteps(this->mc_UnscaledMin, this->mc_UnscaledMax,
                                                               this->mu64_NumberOfStepsAvailable,
                                                               ou32_Index) != C_NO_ERR)
   {
      this->mu64_NumberOfStepsAvailable = 0;
   }
   this->mf64_Factor = of64_Factor;
   this->mf64_Offset = of64_Offset;
   this->mu32_Index = ou32_Index;
   m_Init();
}