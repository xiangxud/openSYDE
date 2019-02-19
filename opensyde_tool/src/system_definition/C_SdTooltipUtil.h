//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to centralize tooltip handling (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDTOOLTIPUTIL_H
#define C_SDTOOLTIPUTIL_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include "C_OSCCanMessage.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdTooltipUtil
{
public:
   static QString h_GetToolTipContentMessage(const stw_opensyde_core::C_OSCCanMessage & orc_Message);
   static QString h_GetToolTipContentSignal(const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                            const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                            const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_DpListElement,
                                            const QString & orc_AutoMinMaxInformation = "",
                                            const QString & orc_AdditionalInformation = "");
   static QString h_ConvertTypeToNameSimplified(const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type & ore_Type);

private:
   C_SdTooltipUtil(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
