//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dark combo-box for settings bar. (implementation)

   Dark combo-box for settings bar..
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QStyledItemDelegate>

#include "C_CamOgeCbxDark.h"

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

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgeCbxDark::C_CamOgeCbxDark(QWidget * const opc_Parent) :
   C_OgeCbxToolTipBase(opc_Parent)
{
   //this code allows to handle the QListView::item in stylesheets
   QStyledItemDelegate * const pc_ItemDelegate = new QStyledItemDelegate();

   this->setItemDelegate(pc_ItemDelegate);
   //lint -e{429}  no memory leak because of the parent all elements and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor.

   \created     16.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgeCbxDark::~C_CamOgeCbxDark()
{
}
