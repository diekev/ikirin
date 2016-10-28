/****************************************************************************
**
 **Copyright (C) 2014
**
 **This file is generated by the Magus toolkit
**
 **THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 **"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 **LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 **A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 **OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 **SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 **LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 **DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 **THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 **(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 **OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#pragma once

#include <QGraphicsView>

#include "widgetbase.h"

#include "scene.h"

class ObjectNodeItem;
class QGraphicsItem;
class QGraphicsRectItem;
class QGraphicsSceneMouseEvent;
class QMenu;
class QtConnection;
class QtNode;
class QtNodeGraphicsScene;
class QtPort;

enum {
	EDITOR_MODE_SCENE  = 0,
	EDITOR_MODE_OBJECT = 1,
};

class NodeView : public QGraphicsView {
public:
	explicit NodeView(QWidget *parent = nullptr);
	explicit NodeView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
	/* Reimplement wheelEvent to avoid getting conflicts between zooming and
	 * scrolling. */
	void wheelEvent(QWheelEvent *event) override;
};

class QtNodeEditor : public WidgetBase {
	Q_OBJECT

	NodeView *m_view;
	QtNodeGraphicsScene *m_graphics_scene;

	QMenu *m_context_menu;
	QMenu *m_zoom_sub_menu;
	QMenu *m_add_node_menu;

	QGraphicsRectItem *m_rubber_band;
	QPointF m_last_mouse_position;

	int m_editor_mode = EDITOR_MODE_SCENE;

	bool m_rubberband_selection;
	bool m_context_menu_enabled;
	bool m_menu_zoom_enabled;
	bool m_menu_collapse_expand_enabled;
	bool m_mouse_down = false;

	/* Cached informations */
	QtConnection *m_hover_connection;
	QtConnection *m_active_connection;
	QVector<QtNode *> m_selected_nodes;
	QVector<QtConnection *> m_selected_connections;

public:
	explicit QtNodeEditor(QWidget *parent = nullptr);
	virtual ~QtNodeEditor();

	/* If true, a context menu is displayed when the right mousebutton is
	 * selected. */
	void setContextMenuEnabled(bool enabled);
	bool isContextMenuEnabled();

	/* If true, the context menu is extended with an option to zoom in/out. */
	void setMenuZoomEnabled(bool enabled);
	bool isMenuZoomEnabled();

	/* If true, the context menu is extended with an option to exand/collapse
	 * all nodes. */
	void setMenuCollapseExpandEnabled(bool enabled);
	bool isMenuCollapseExpandEnabled();

	/* Add a node to the scene (editor widget). */
	void addNode(QtNode *node);

	/* Add a node to the scene (editor widget). */
	QVector<QtNode *> getNodes() const;

	/* Remove a node from the scene and destroy it. */
	void removeNode(QtNode *node);

	/* Removes (and destroys) all selected nodes and connections from the scene
	 * (editor widget). */
	void removeAllSelelected();

	/* Center all nodes */
	void center();

	/* Remove a connection from the scene (editor widget) and destroy it. */
	void removeConnection(QtConnection *connection);

	/* Remove AND delete all items from the node editor scene. */
	void clear();

	/* If there is a selected node, return the last one that was selected. */
	QtNode *getLastSelectedNode() const;

	/* Return a vector with all the currently selected nodes. */
	const QVector<QtNode *> &selectedNodes() const;

	/* Return a vector with all the currently selected connections. */
	const QVector<QtConnection *> &selectedConnections() const;

	/* Moves a node to front; before all other nodes. */
	void toFront(QtNode *node);

	/* Moves all other nodes in front of the given node. */
	void toBack(QtNode *node);

	/* Scale all nodes with the zoom factor. */
	void setZoom(qreal zoom);

	QtConnection *nodeOverConnection(QtNode *node);
	QtConnection *lastSelectedConnection() const;

	void setAddNodeMenu(QMenu *menu)
	{
		m_add_node_menu = menu;
	}

	/* Called for creating new connections, e.g. during node dropping. */
	void connectNodes(QtNode *from, QtPort *from_sock, QtNode *to, QtPort *to_sock, bool notify, bool connect_graph = true);

	int editor_mode() const
	{
		return m_editor_mode;
	}

	void update_state(event_type event) override;

	void sendNotification() const;

public Q_SLOTS:
	/* Activated when a contextmenu item is selected */
	void contextMenuItemSelected(QAction *action);

	/* Activated when a connection is set between two nodes. */
	void connectionEstablished(QtConnection*);

private:
	/* Called when a node is dropped on a connection. */
	void splitConnectionWithNode(QtNode *node);

	/* Called when an object node is selected. */
	void setActiveObject(ObjectNodeItem *node);

	/* Called when a node is removed. */
	void removeNodeEx(QtNode *node);

	/* Called when nodes are connected. */
	void nodesConnected(QtNode *from, const QString &socket_from, QtNode *to, const QString &socket_to, bool notify);

	/* Called when nodes are disconnected. */
	void connectionRemoved(QtNode *from, const QString &socket_from, QtNode *to, const QString &socket_to, bool notify);

	void enterObjectNode(QAction *action);

protected:
	/* Event handling */
	bool eventFilter(QObject *object, QEvent *event) override;

	bool mouseClickHandler(QGraphicsSceneMouseEvent *mouseEvent);
	bool mouseDoubleClickHandler(QGraphicsSceneMouseEvent *mouseEvent);
	bool mouseMoveHandler(QGraphicsSceneMouseEvent *mouseEvent);
	bool mouseReleaseHandler(QGraphicsSceneMouseEvent *mouseEvent);

	bool ctrlPressed();
	void keyPressEvent(QKeyEvent *event) override;

	QGraphicsItem *itemAtExceptActiveConnection(const QPointF &pos);

	void rubberbandSelection(QGraphicsSceneMouseEvent *mouseEvent);
	void deselectAll();
	void deleteAllActiveConnections();
	void deselectConnections();
	void deselectNodes();
	void selectNode(QtNode *node, QGraphicsSceneMouseEvent *mouseEvent);
	void selectConnection(QtConnection *connection);

	QtNode *nodeWithActiveConnection();

	bool isAlreadySelected(QtNode *node);
	bool isAlreadySelected(QtConnection *connection);

	void showContextMenu(const QPoint &pos);
	QAction *getActionFromContextMenu(const QString &actionText);

	void setZoomForAction(qreal zoom, QAction *action);
	void resetZoomSubmenu();
};
