//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Table view with tool tip (implementation)

   Table view with tool tip

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QHelpEvent>
#include <QHeaderView>
#include <QHoverEvent>
#include "stwtypes.h"
#include "constants.h"
#include "C_TblViewToolTipBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;

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

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_TblViewToolTipBase::C_TblViewToolTipBase(QWidget * const opc_Parent) :
   QTableView(opc_Parent),
   mpc_ToolTip(NULL),
   ms32_HoveredRow(-1),
   ms32_HoveredCol(-1),
   ms32_HoveredVertHeader(-1),
   ms32_HoveredHorzHeader(-1)
{
   //Check mouse tracking active in all tables using this class?
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     17.08.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_TblViewToolTipBase::~C_TblViewToolTipBase(void)
{
   delete mpc_ToolTip;
   mpc_ToolTip = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   Here: Handle tooltip if necessary

   \param[in,out] opc_Event Event identification and information

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblViewToolTipBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   QTableView::mouseMoveEvent(opc_Event);
   m_HandleMouseMoveToolTip(opc_Event->globalPos());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_TblViewToolTipBase::event(QEvent * const opc_Event)
{
   bool q_Return;

   if (opc_Event->type() == QEvent::ToolTip)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QHelpEvent * const pc_HelpEvent = dynamic_cast<QHelpEvent * const>(opc_Event);

      if (pc_HelpEvent != NULL)
      {
         //Never use global pos directly as this seems to ignore transformations
         const QPoint c_GlobalPos = this->mapToGlobal(pc_HelpEvent->pos());
         //show tooltip
         if (mpc_ToolTip == NULL)
         {
            mpc_ToolTip = new C_NagToolTip();
         }

         if (this->mpc_ToolTip->isVisible() == false)
         {
            //Check first header
            if ((this->verticalHeader()->isVisible() == true) &&
                (this->verticalHeader()->geometry().contains(
                    this->verticalHeader()->mapFromGlobal(c_GlobalPos)) == true))
            {
               const sint32 s32_LogicalIndex =
                  this->verticalHeader()->logicalIndexAt(this->verticalHeader()->mapFromGlobal(c_GlobalPos));
               const sint32 s32_VisualIndex = this->verticalHeader()->visualIndex(s32_LogicalIndex);
               if (s32_VisualIndex >= 0)
               {
                  const QString c_Heading = this->model()->headerData(s32_VisualIndex, Qt::Vertical,
                                                                      msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->model()->headerData(s32_VisualIndex, Qt::Vertical,
                                                                      msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();

                  if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredVertHeader = s32_VisualIndex;
                     //Update text
                     this->setMouseTracking(true);
                     this->mpc_ToolTip->SetHeading(c_Heading);
                     this->mpc_ToolTip->SetContent(c_Content);
                     this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);
                     this->mpc_ToolTip->show();
                     //Use global pos for move because that's the actual target position
                     this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                  }
               }
            }
            //Check second header
            else if ((this->horizontalHeader()->isVisible() == true) &&
                     (this->horizontalHeader()->geometry().contains(
                         this->horizontalHeader()->mapFromGlobal(c_GlobalPos)) == true))
            {
               const sint32 s32_LogicalIndex =
                  this->horizontalHeader()->logicalIndexAt(this->horizontalHeader()->mapFromGlobal(c_GlobalPos));
               const sint32 s32_VisualIndex = this->horizontalHeader()->visualIndex(s32_LogicalIndex);
               if (s32_VisualIndex >= 0)
               {
                  const QString c_Heading = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                  const QString c_Content = this->model()->headerData(s32_VisualIndex, Qt::Horizontal,
                                                                      msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();

                  if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                  {
                     this->ms32_HoveredHorzHeader = s32_VisualIndex;
                     //Update text
                     this->setMouseTracking(true);
                     this->mpc_ToolTip->SetHeading(c_Heading);
                     this->mpc_ToolTip->SetContent(c_Content);
                     this->mpc_ToolTip->SetType(C_NagToolTip::eDEFAULT);
                     this->mpc_ToolTip->show();
                     //Use global pos for move because that's the actual target position
                     this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                  }
               }
            }
            else
            {
               //Check table last!
               const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(c_GlobalPos));
               if (c_Index.isValid() == true)
               {
                  const sint32 s32_ToolTipRow = c_Index.row();
                  const sint32 s32_ToolTipCol = c_Index.column();
                  if ((s32_ToolTipRow >= 0) && (s32_ToolTipCol >= 0))
                  {
                     const QString c_Heading = c_Index.data(msn_USER_ROLE_TOOL_TIP_HEADING).value<QString>();
                     const QString c_Content = c_Index.data(msn_USER_ROLE_TOOL_TIP_CONTENT).value<QString>();
                     const C_NagToolTip::E_Type e_Type =
                        static_cast<C_NagToolTip::E_Type>(c_Index.data(msn_USER_ROLE_TOOL_TIP_TYPE).toInt());
                     if ((c_Heading.compare("") != 0) || (c_Content.compare("") != 0))
                     {
                        this->ms32_HoveredRow = s32_ToolTipRow;
                        this->ms32_HoveredCol = s32_ToolTipCol;
                        //Update text
                        this->setMouseTracking(true);
                        this->mpc_ToolTip->SetHeading(c_Heading);
                        this->mpc_ToolTip->SetContent(c_Content);
                        this->mpc_ToolTip->SetType(e_Type);
                        this->mpc_ToolTip->show();
                        //Use global pos for move because that's the actual target position
                        this->mpc_ToolTip->DoMove(pc_HelpEvent->globalPos());
                     }
                  }
               }
            }
         }
         else
         {
            m_HandleMouseMoveToolTip(c_GlobalPos);
         }
      }

      opc_Event->accept();
      q_Return = true;
   }
   else if (opc_Event->type() == QEvent::Leave)
   {
      //hide on leave
      m_HideTooltip();
      q_Return = QTableView::event(opc_Event);
   }
   //Necessary to detect mouse move events over integrated widgets
   else if (opc_Event->type() == QEvent::HoverMove)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);

      if (pc_HoverEvent != NULL)
      {
         m_HandleMouseMoveToolTip(this->mapToGlobal(pc_HoverEvent->pos()));
      }
      q_Return = QTableView::event(opc_Event);
   }
   else
   {
      q_Return = QTableView::event(opc_Event);
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Hide tooltip

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblViewToolTipBase::m_HideTooltip(void)
{
   if (this->mpc_ToolTip != NULL)
   {
      this->mpc_ToolTip->hide();
   }
   this->ms32_HoveredRow = -1;
   this->ms32_HoveredCol = -1;
   this->ms32_HoveredVertHeader = -1;
   this->ms32_HoveredHorzHeader = -1;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle mouse move (tool tip related)

   \param[in] orc_GlobalPos Global mouse pos

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_TblViewToolTipBase::m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos)
{
   if ((this->ms32_HoveredRow >= 0) && (this->ms32_HoveredCol >= 0))
   {
      const QModelIndex c_Index = this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
      if (c_Index.isValid() == true)
      {
         if ((this->ms32_HoveredRow != c_Index.row()) || (this->ms32_HoveredCol != c_Index.column()))
         {
            //Hide Tooltip
            m_HideTooltip();
         }
      }
      else
      {
         //Hide Tooltip
         m_HideTooltip();
      }
   }
   if ((this->verticalHeader()->isVisible() == true) && (this->ms32_HoveredVertHeader >= 0))
   {
      const sint32 s32_LogicalIndex =
         this->verticalHeader()->logicalIndexAt(this->verticalHeader()->mapFromGlobal(orc_GlobalPos));
      const sint32 s32_VisualIndex = this->verticalHeader()->visualIndex(s32_LogicalIndex);
      if (s32_VisualIndex != this->ms32_HoveredVertHeader)
      {
         //Hide Tooltip
         m_HideTooltip();
      }
      else
      {
         if (this->verticalHeader()->geometry().contains(
                this->verticalHeader()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideTooltip();
         }
      }
   }
   if ((this->horizontalHeader()->isVisible() == true) && (this->ms32_HoveredHorzHeader >= 0))
   {
      const sint32 s32_LogicalIndex =
         this->horizontalHeader()->logicalIndexAt(this->horizontalHeader()->mapFromGlobal(orc_GlobalPos));
      const sint32 s32_VisualIndex = this->horizontalHeader()->visualIndex(s32_LogicalIndex);
      if (s32_VisualIndex != this->ms32_HoveredHorzHeader)
      {
         //Hide Tooltip
         m_HideTooltip();
      }
      else
      {
         if (this->horizontalHeader()->geometry().contains(
                this->horizontalHeader()->mapFromGlobal(orc_GlobalPos)) == false)
         {
            //Hide Tooltip
            m_HideTooltip();
         }
      }
   }
}
