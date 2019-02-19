//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Add new items based on snapshot information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAUNOADDSNAPSHOTCOMMAND_H
#define C_SYVDAUNOADDSNAPSHOTCOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include "C_SyvDaUnoAddBaseCommand.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaUnoAddSnapshotCommand :
   public C_SyvDaUnoAddBaseCommand
{
public:
   C_SyvDaUnoAddSnapshotCommand(QGraphicsScene * const opc_Scene, const C_PuiSvDashboard & orc_InitialSnapshotData,
                                const std::vector<stw_types::uint64> & orc_IDs,
                                const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                           C_PuiSvReadDataConfiguration> & orc_RestoredRails,
                                const QPointF & orc_NewPos,
                                QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SyvDaUnoAddSnapshotCommand(void);

protected:
   virtual void m_AddNew(void);

private:
   const QPointF mc_NewPos;

   void m_InitialReadRailHandling(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif