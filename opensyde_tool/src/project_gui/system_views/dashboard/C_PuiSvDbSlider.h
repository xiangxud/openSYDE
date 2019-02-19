//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard slider data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVDBSLIDER_H
#define C_PUISVDBSLIDER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_PuiSvDbWidgetBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvDbSlider :
   public C_PuiSvDbWidgetBase
{
public:
   enum E_Type
   {
      eTYPE_SMALL_COLOR_1,
      eTYPE_BIG_COLOR_1,
      eTYPE_SMALL_COLOR_2,
      eTYPE_BIG_COLOR_2
   };

   C_PuiSvDbSlider(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const;

   E_Type e_Type;
   bool q_ShowMinMax;
   stw_types::sint32 s32_Value;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif