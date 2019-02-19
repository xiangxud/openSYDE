//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABLISTHEADER_H
#define C_OGELABLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabListHeader :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif