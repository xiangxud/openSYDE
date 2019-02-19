//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data set edit widget

   Data set edit widget

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_SdNdeDataPoolListDataSetWidget.h"
#include "ui_C_SdNdeDataPoolListDataSetWidget.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Data pool index
   \param[in]     oru32_ListIndex     List index

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetWidget::C_SdNdeDataPoolListDataSetWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 & oru32_NodeIndex,
   const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDataPoolListDataSetWidget),
   mpc_ContextMenu(NULL),
   mpc_ActionAdd(NULL),
   mpc_ActionDelete(NULL),
   mpc_ActionCopy(NULL),
   mpc_ActionPaste(NULL),
   mpc_ActionCut(NULL),
   mpc_ActionMoveLeft(NULL),
   mpc_ActionMoveRight(NULL),
   mrc_Parent(orc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_ListIndex(oru32_ListIndex)
{
   const stw_opensyde_core::C_OSCNodeDataPoolList * const pc_List =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(oru32_NodeIndex,
                                                          oru32_DataPoolIndex,
                                                          oru32_ListIndex);

   this->mpc_Ui->setupUi(this);
   this->mrc_Parent.SetWidget(this);
   InitStaticNames();
   this->mpc_Ui->pc_TableView->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);

   //Deactivate debug title
   this->mpc_Ui->pc_GroupBoxEmpty->setTitle("");

   //Custom context menu
   m_SetupContextMenu();

   // Init icons
   m_InitButtonIcons();

   //Initial display
   if (pc_List != NULL)
   {
      m_OnDataSetCountChange(pc_List->c_DataSets.size());
   }
   m_OnDataSetSelectionChange(0);

   //Connects
   connect(this->mpc_Ui->pc_BushButtonOk, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDataPoolListDataSetWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDataPoolListDataSetWidget::m_CancelClicked);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::SigColumnCountChange, this,
           &C_SdNdeDataPoolListDataSetWidget::m_OnDataSetCountChange);
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::SigSelectionChanged, this,
           &C_SdNdeDataPoolListDataSetWidget::m_OnDataSetSelectionChange);

   //Connect buttons
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SdNdeDataPoolListDataSetWidget::m_DoInsert);
   connect(this->mpc_Ui->pc_PushButtonCopy, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::Copy);
   connect(this->mpc_Ui->pc_PushButtonCut, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::Cut);
   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::Delete);
   connect(this->mpc_Ui->pc_PushButtonMoveRight, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::DoMoveRight);
   connect(this->mpc_Ui->pc_PushButtonMoveLeft, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::DoMoveLeft);
   connect(this->mpc_Ui->pc_PushButtonPaste, &QPushButton::clicked, this->mpc_Ui->pc_TableView,
           &C_SdNdeDataPoolListDataSetView::Paste);
   //lint -e{64,918,1025,1703} Qt interface
   connect(this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::SigButtonChange, this,
           &C_SdNdeDataPoolListDataSetWidget::m_HandleButtonChange);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetWidget::~C_SdNdeDataPoolListDataSetWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent and the Qt memory management or never took ownership
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::InitStaticNames(void) const
{
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex);

   if (pc_List != NULL)
   {
      mrc_Parent.SetTitle(QString(C_GtGetText::h_GetText("List %1")).arg(pc_List->c_Name.c_str()));
   }
   mrc_Parent.SetSubTitle(QString(C_GtGetText::h_GetText("Dataset Configuration")));
   this->mpc_Ui->pc_LabelReplacement->setText(C_GtGetText::h_GetText(
                                                 "No Dataset is declared, add any via the '+' button"));
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"), "");
   this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Copy"), "");
   this->mpc_Ui->pc_PushButtonCut->SetToolTipInformation(C_GtGetText::h_GetText("Cut"), "");
   this->mpc_Ui->pc_PushButtonPaste->SetToolTipInformation(C_GtGetText::h_GetText("Paste"), "");
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"), "");
   this->mpc_Ui->pc_PushButtonMoveLeft->SetToolTipInformation(C_GtGetText::h_GetText("Move left"), "");
   this->mpc_Ui->pc_PushButtonMoveRight->SetToolTipInformation(C_GtGetText::h_GetText("Move right"), "");
}

//-----------------------------------------------------------------------------
/*!
   \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QUndoCommand * C_SdNdeDataPoolListDataSetWidget::TakeUndoCommand(void) const
{
   return this->mpc_Ui->pc_TableView->TakeUndoCommand();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set model view manager

   \param[in,out] opc_Value Model view manager

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::SetModelViewManager(C_SdNdeDataPoolListModelViewManager * const opc_Value) const
{
   this->mpc_Ui->pc_TableView->SetModelViewManager(opc_Value);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: handle list paste

   \param[in,out] opc_Event Event identification and information

   \created     26.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   bool q_CallOrig = true;

   //Handle all enter key cases manually
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_Event->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_Event->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_Event->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_Event->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_Parent.accept();
      }
      else
      {
         q_CallOrig = false;
      }
   }
   else
   {
      if (this->mpc_Ui->pc_GroupBoxGeneral->isAncestorOf(this->focusWidget()) == true)
      {
         const sintn sn_Key = opc_Event->key();
         switch (sn_Key)
         {
         case Qt::Key_C:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Copy();
               opc_Event->accept();
            }
            break;
         case Qt::Key_X:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Cut();
               opc_Event->accept();
            }
            break;
         case Qt::Key_V:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Paste();
               opc_Event->accept();
            }
            break;
         case Qt::Key_Delete:
            q_CallOrig = false;
            this->mpc_Ui->pc_TableView->Delete();
            opc_Event->accept();
            break;
         case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
         case Qt::Key_Plus:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->Insert(false);
               opc_Event->accept();
            }
            break;
         case Qt::Key_Left:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->DoMoveLeft();
               opc_Event->accept();
            }
            break;
         case Qt::Key_Right:
            if (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true)
            {
               q_CallOrig = false;
               this->mpc_Ui->pc_TableView->DoMoveRight();
               opc_Event->accept();
            }
            break;
         default:
            //Nothing to do
            break;
         }
      }
   }
   if (q_CallOrig == true)
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize icons of buttons for all states.

   \created     10.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_InitButtonIcons(void) const
{
   // TODO icon states: svg icons move left and move right; 4 different states
   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "://images/IconAddHovered.svg",
                                                  "://images/IconAddClicked.svg", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonCopy->SetCustomIcons("://images/system_definition/NodeEdit/lists/Copy.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyHovered.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyClicked.svg",
                                                   "://images/system_definition/NodeEdit/lists/CopyDisabled.svg");
   this->mpc_Ui->pc_PushButtonCut->SetCustomIcons("://images/system_definition/NodeEdit/lists/Cut.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutHovered.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutClicked.svg",
                                                  "://images/system_definition/NodeEdit/lists/CutDisabled.svg");
   this->mpc_Ui->pc_PushButtonDelete->SetCustomIcons("://images/main_page_and_navi_bar/Icon_delete.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_hover.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_clicked.svg",
                                                     "://images/main_page_and_navi_bar/Icon_delete_disabled.svg");
   this->mpc_Ui->pc_PushButtonMoveLeft->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg",
                                                       "://images/system_definition/NodeEdit/lists/MoveLeft.svg");
   this->mpc_Ui->pc_PushButtonMoveRight->SetCustomIcons("://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg",
                                                        "://images/system_definition/NodeEdit/lists/MoveRight.svg");
   this->mpc_Ui->pc_PushButtonPaste->SetCustomIcons("://images/system_definition/NodeEdit/lists/Paste.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteHovered.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteClicked.svg",
                                                    "://images/system_definition/NodeEdit/lists/PasteDisabled.svg");
}

//-----------------------------------------------------------------------------
/*!
   \brief   On ok clicked

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_OkClicked(void)
{
   this->mrc_Parent.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On cancel clicked

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_CancelClicked(void)
{
   this->mrc_Parent.reject();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set buttons status

   \param[in] orq_AddActive       Push button add active
   \param[in] orq_CutActive       Push button cut active
   \param[in] orq_CopyActive      Push button copy active
   \param[in] orq_PasteActive     Push button paste active
   \param[in] orq_DeleteActive    Push button delete active
   \param[in] orq_MoveLeftActive  Push button move left active
   \param[in] orq_MoveRightActive Push button move right active

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive,
                                                            const bool & orq_CopyActive, const bool & orq_PasteActive,
                                                            const bool & orq_DeleteActive,
                                                            const bool & orq_MoveLeftActive,
                                                            const bool & orq_MoveRightActive)
{
   this->mpc_Ui->pc_PushButtonAdd->setEnabled(orq_AddActive);
   if (this->mpc_ActionAdd != NULL)
   {
      this->mpc_ActionAdd->setEnabled(orq_AddActive);
   }
   this->mpc_Ui->pc_PushButtonCut->setEnabled(orq_CutActive);
   if (this->mpc_ActionCut != NULL)
   {
      this->mpc_ActionCut->setEnabled(orq_CutActive);
   }
   this->mpc_Ui->pc_PushButtonCopy->setEnabled(orq_CopyActive);
   if (this->mpc_ActionCopy != NULL)
   {
      this->mpc_ActionCopy->setEnabled(orq_CopyActive);
   }
   this->mpc_Ui->pc_PushButtonPaste->setEnabled(orq_PasteActive);
   if (this->mpc_ActionPaste != NULL)
   {
      this->mpc_ActionPaste->setEnabled(orq_PasteActive);
   }
   this->mpc_Ui->pc_PushButtonDelete->setEnabled(orq_DeleteActive);
   if (this->mpc_ActionDelete != NULL)
   {
      this->mpc_ActionDelete->setEnabled(orq_DeleteActive);
   }
   this->mpc_Ui->pc_PushButtonMoveLeft->setEnabled(orq_MoveLeftActive);
   if (this->mpc_ActionMoveLeft != NULL)
   {
      this->mpc_ActionMoveLeft->setEnabled(orq_MoveLeftActive);
   }
   this->mpc_Ui->pc_PushButtonMoveRight->setEnabled(orq_MoveRightActive);
   if (this->mpc_ActionMoveRight != NULL)
   {
      this->mpc_ActionMoveRight->setEnabled(orq_MoveRightActive);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle data set count change

   \param[in] ors32_NewColumnCount New column count

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_OnDataSetCountChange(const sint32 & ors32_NewColumnCount) const
{
   if (ors32_NewColumnCount > 0)
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxEmpty->setVisible(true);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle data set selction change

   \param[in] oru32_SelectionCount Selected item count

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_OnDataSetSelectionChange(const uint32 & oru32_SelectionCount) const
{
   QString c_Text;

   //Handle selection label
   if (oru32_SelectionCount == 0)
   {
      c_Text = "";
   }
   else if (oru32_SelectionCount == 1)
   {
      c_Text = QString(C_GtGetText::h_GetText("1 Dataset selected"));
   }
   else
   {
      c_Text = QString(C_GtGetText::h_GetText("%1 Datasets selected")).arg(oru32_SelectionCount);
   }
   this->mpc_Ui->pc_SelectionLabel->setText(c_Text);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init context menu entries

   \created     20.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new stw_opensyde_gui_elements::C_OgeContextMenu(this);

   this->mpc_ActionAdd = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Add new Dataset"), this, &C_SdNdeDataPoolListDataSetWidget::m_DoInsert,
      static_cast<sintn>(Qt::CTRL) +
      static_cast<sintn>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionCut = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Cut"), this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::Cut,
      static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_ActionCopy = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Copy"), this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::Copy,
      static_cast<sintn>(Qt::CTRL) +
      static_cast<sintn>(Qt::Key_C));
   this->mpc_ActionPaste = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Paste"), this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::Paste,
      static_cast<sintn>(Qt::CTRL) +
      static_cast<sintn>(Qt::Key_C));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionMoveLeft = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                  "Move Left"), this->mpc_Ui->pc_TableView,
                                                               &C_SdNdeDataPoolListDataSetView::DoMoveLeft,
                                                               static_cast<sintn>(Qt::CTRL) +
                                                               static_cast<sintn>(Qt::Key_Left));
   this->mpc_ActionMoveRight = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText(
                                                                   "Move Right"), this->mpc_Ui->pc_TableView,
                                                                &C_SdNdeDataPoolListDataSetView::DoMoveRight,
                                                                static_cast<sintn>(Qt::CTRL) +
                                                                static_cast<sintn>(Qt::Key_Right));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ActionDelete = this->mpc_ContextMenu->addAction(
      C_GtGetText::h_GetText(
         "Delete"), this->mpc_Ui->pc_TableView, &C_SdNdeDataPoolListDataSetView::Delete,
      static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdNdeDataPoolListDataSetWidget::customContextMenuRequested, this,
           &C_SdNdeDataPoolListDataSetWidget::m_OnCustomContextMenuRequested);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position

   \created     20.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   const QPoint c_Global = this->mapToGlobal(orc_Pos);

   if (this->mpc_Ui->pc_GroupBoxGeneral->rect().contains(this->mpc_Ui->pc_GroupBoxGeneral->mapFromGlobal(c_Global)) ==
       true)
   {
      this->mpc_ContextMenu->popup(c_Global);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle insert action

   \created     13.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListDataSetWidget::m_DoInsert(void) const
{
   this->mpc_Ui->pc_TableView->Insert(true);
}
