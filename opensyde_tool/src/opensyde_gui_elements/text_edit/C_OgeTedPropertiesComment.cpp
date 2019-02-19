//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Text edit field for property comment (implementation)

   Text edit field for property comment.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeTedPropertiesComment.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     16.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------

C_OgeTedPropertiesComment::C_OgeTedPropertiesComment(QWidget * const opc_Parent) :
   C_OgeTedToolTipBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten focus out event slot

   Here: clear text selection

   \param[in,out] opc_Event Event identification and information

   \created     16.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_OgeTedPropertiesComment::focusOutEvent(QFocusEvent * const opc_Event)
{
   QTextCursor c_TempTextCursor;

   if (opc_Event->reason() != Qt::PopupFocusReason)
   {
      c_TempTextCursor = this->textCursor();

      c_TempTextCursor.clearSelection();
      this->setTextCursor(c_TempTextCursor);
   }

   QTextEdit::focusOutEvent(opc_Event);
}