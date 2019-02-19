//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Central handling for errors (implementation)

   Central handling for errors

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEventLoop>
#include "stwerrors.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_OgeWiError.h"
#include "C_OgePopUpDialog.h"
#include "C_PopErrorHandling.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
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
   \brief   Load with error handling

   \param[in]   ors32_Err     Load error
   \param[in]   orc_Path      path of project to load
   \param[in]   opc_Parent    parent widget

   \created     15.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_PopErrorHandling::mh_ProjectLoadErr(const sint32 & ors32_Err, const QString & orc_Path,
                                           QWidget * const opc_Parent)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project load"));
      c_Message.SetDescription(orc_Path);
      switch (ors32_Err)
      {
      case C_RD_WR:
         c_Message.SetDetails(C_GtGetText::h_GetText("There are problems accessing the file system.\n"
                                                     "For example, there may be no read access to the file."));
         c_Message.Execute();
         break;
      case C_RANGE:
         c_Message.SetDetails(C_GtGetText::h_GetText("At least one project file is missing."));
         c_Message.Execute();
         break;
      case C_NOACT:
         c_Message.SetDetails(C_GtGetText::h_GetText("A project file is present but its structure is invalid.\n"
                                                     "For example this can be caused by an invalid XML file."));
         c_Message.Execute();
         break;
      case C_CONFIG:
         c_Message.SetDetails(C_GtGetText::h_GetText("The content of a project file is invalid or incomplete."));
         c_Message.Execute();
         break;
      case C_COM:
         //Update log file
         C_OSCLoggingHandler::h_Flush();
         c_Message.SetDetails(QString("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                              arg(C_GtGetText::h_GetText(
                                     "The device definition for the project was not found. For more information see ")).
                              arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                              arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                              arg(C_GtGetText::h_GetText("log file")));
         c_Message.Execute();
         break;
      default:
         c_Message.SetDetails(C_GtGetText::h_GetText("Unknown cause."));
         c_Message.Execute();
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save with error handling

   \param[in]   ors32_Err     Save error
   \param[in]   opc_Parent    parent widget

   \created     15.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

void C_PopErrorHandling::mh_ProjectSaveErr(const sint32 & ors32_Err, QWidget * const opc_Parent)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::E_Type::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project save"));
      switch (ors32_Err)
      {
      case C_RD_WR:
         c_Message.SetDescription(C_GtGetText::h_GetText("Problems accessing file system."));
         break;
      case C_RANGE:
         c_Message.SetDescription(C_GtGetText::h_GetText("Path is empty."));
         break;
      case C_NOACT:
         c_Message.SetDescription(C_GtGetText::h_GetText("Could not create project directory."));
         break;
      case C_COM:
         c_Message.SetDescription(C_GtGetText::h_GetText("Message sorting failed."));
         break;
      default:
         c_Message.SetDescription(C_GtGetText::h_GetText("Unknown cause."));
         break;
      }
      c_Message.Execute();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle gettext initialize error

   \param[in]   ors32_Err   Error

   \created     29.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PopErrorHandling::mh_GetTextInitializeErr(const sint32 & ors32_Err)
{
   if (ors32_Err != C_NO_ERR)
   {
      C_OgeWiError * pc_Widget = new C_OgeWiError();
      C_OgePopUpDialog c_Dialog;
      c_Dialog.SetTitle("Error occurred!");
      c_Dialog.SetWidget(pc_Widget);
      pc_Widget->SaveParentDialog(&c_Dialog);

      switch (ors32_Err)
      {
      case C_CONFIG:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OSCGetText - h_Initialize:\n"
                                                               "orc_BasePath does not point to a valid directory"));
         c_Dialog.exec();
         break;
      case C_NOACT:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OSCGetText - h_Initialize:\n"
                                                               "internal error in gettext library"));
         c_Dialog.exec();
         break;
      default:
         pc_Widget->SetErrorDescription(C_GtGetText::h_GetText("C_OSCGetText - h_Initialize: unknown error"));
         c_Dialog.exec();
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   \created     15.07.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PopErrorHandling::C_PopErrorHandling()
{
}