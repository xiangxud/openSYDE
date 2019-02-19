//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Text element for system view bus (implementation)

   Text element for system view bus

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_GiSvTextElementBus.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]       ors32_Index          Index of data element in system definition
   \param[in]       oru64_ID             Unique ID
   \param[in,out]   opc_parent           Optional pointer to parent

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvTextElementBus::C_GiSvTextElementBus(const sint32 & ors32_Index, const uint64 & oru64_ID,
                                           QGraphicsItem * const opc_Parent) :
   C_GiTextElementBus(ors32_Index, oru64_ID, opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle tooltip content

   \created     15.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvTextElementBus::GenerateHint(void)
{
   if ((this->mq_ErrorIconHovered == true) && (this->ms32_Index >= 0))
   {
      this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Invalid Bus Content"));
      this->SetDefaultToolTipContent(C_GtGetText::h_GetText("For further details switch to system definition"));
      this->SetDefaultToolTipType(C_NagToolTip::eERROR);
   }
   else
   {
      this->SetDefaultToolTipHeading("");
      this->SetDefaultToolTipContent("");
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
   }
}
