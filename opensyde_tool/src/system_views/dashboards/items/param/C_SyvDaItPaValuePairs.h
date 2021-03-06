//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to model value pairs for parametrization write process (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAVALUEPAIRS_H
#define C_SYVDAITPAVALUEPAIRS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaValuePairs
{
public:
   C_SyvDaItPaValuePairs(void);
   C_SyvDaItPaValuePairs(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Expected, const bool oq_Changed);

   bool q_Changed;
   stw_opensyde_core::C_OSCNodeDataPoolContent c_Expected;
   stw_opensyde_core::C_OSCNodeDataPoolContent c_Actual;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
