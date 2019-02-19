//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list table drawing delegate (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEDELEGATE_H
#define C_SDNDEDATAPOOLLISTTABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QStyledItemDelegate>
#include <QUndoStack>
#include "C_SdNdeDataPoolListTableModel.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListTableDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListTableDelegate(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const override;
   virtual void destroyEditor(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const override;
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   //lint -restore

   void SetModel(C_SdNdeDataPoolListTableModel * const opc_Value);

   void SetUndoStack(QUndoStack * const opc_Value);
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigLinkClicked(const QModelIndex & orc_Index) const;
   void SigDestroyEditor(void) const;
   void SigCreateEditor(const QModelIndex & orc_Index) const;
   void SigChangeAutoMinMax(const bool & orq_Checked) const;
   void SigSetModelData(void) const;

private:
   C_SdNdeDataPoolListTableModel * mpc_Model;
   QUndoStack * mpc_UndoStack;
   const QPixmap mc_DisabledPixmapDark;
   const QPixmap mc_DisabledPixmapLight;
   const QPixmap mc_CheckMark;
   stw_types::sint32 ms32_EditCount;
   stw_types::sint32 ms32_UndoStartIndex;
   QModelIndex mc_Edit;
   bool mq_ChangeInProgress;
   bool mq_Inital;
   bool mq_ChangeDetected;

   QWidget * m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                            const C_SdNdeDataPoolListTableModel::E_Columns oe_Col) const;
   void m_OnDestroyEditor(void);
   void m_OnCreateEditor(const QModelIndex & orc_Index);
   void m_OnNameChange(const QString & orc_Text) const;
   void m_OnAutoMinMaxChange(const bool & orq_Checked);
   void m_OnSetModelData(void);
   void m_OnValueTypeChange(const stw_types::sintn & orsn_Index);
   void m_OnArraySizeChange(const stw_types::sintn & orsn_NewSize);
   void m_PrepareForNewOne(void);
   void m_CleanUpLastOne(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
