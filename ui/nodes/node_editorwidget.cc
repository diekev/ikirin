/****************************************************************************
**
* *Copyright (C) 2014
**
* *This file is generated by the Magus toolkit
**
* *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* *"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* *LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* *A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* *OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* *SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* *LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* *DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* *THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "node_editorwidget.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QVBoxLayout>

#include <iostream>

#include "node_compound.h"
#include "node_connection.h"
#include "node_node.h"
#include "node_port.h"
#include "node_scene.h"

static constexpr auto NODE_ACTION_DELETE = "Delete selected items";
static constexpr auto NODE_ACTION_CENTER = "Center";
static constexpr auto NODE_ACTION_ZOOM = "Zoom (%)";
static constexpr auto NODE_ACTION_ZOOM_10 = "10%";
static constexpr auto NODE_ACTION_ZOOM_25 = "25%";
static constexpr auto NODE_ACTION_ZOOM_50 = "50%";
static constexpr auto NODE_ACTION_ZOOM_75 = "75%";
static constexpr auto NODE_ACTION_ZOOM_90 = "90%";
static constexpr auto NODE_ACTION_ZOOM_100 = "100%";
static constexpr auto NODE_ACTION_ZOOM_150 = "150%";
static constexpr auto NODE_ACTION_ZOOM_200 = "200%";
static constexpr auto NODE_ACTION_ZOOM_250 = "250%";
static constexpr auto NODE_ACTION_ZOOM_300 = "300%";
static constexpr auto NODE_ACTION_COLLAPSE_ALL = "Collapse all nodes";
static constexpr auto NODE_ACTION_EXPAND_ALL = "Expand all nodes";
static constexpr auto NODE_ENTER_OBJECT = "Enter object";
static constexpr auto NODE_EXIT_OBJECT = "Exit object";

//****************************************************************************/
static inline bool is_object_node(QGraphicsItem *item)
{
	if (item->data(NODE_KEY_GRAPHIC_ITEM_SUBTYPE).isValid()) {
		int type = item->data(NODE_KEY_GRAPHIC_ITEM_SUBTYPE).toInt();
		return type == NODE_VALUE_SUBTYPE_OBJECT;
	}

	return false;
}

static inline bool is_node(QGraphicsItem *item)
{
	if (item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).isValid()) {
		int type = item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).toInt();
		return NODE_VALUE_TYPE_NODE == type;
	}

	return false;
}

static inline bool is_connection(QGraphicsItem *item)
{
	if (item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).isValid()) {
		int type = item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).toInt();
		return NODE_VALUE_TYPE_CONNECTION == type;
	}

	return false;
}

//****************************************************************************/
QtNodeEditor::QtNodeEditor(QWidget *parent)
    : QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	m_view = new NodeView(this);

	m_scene_scene = new QtNodeGraphicsScene();
	m_scene_scene->installEventFilter(this);
	m_current_scene = m_scene_scene;

	m_view->setScene(m_current_scene);

	m_view->setRenderHint(QPainter::Antialiasing, true);
	m_view->setInteractive(true);
	m_view->setMouseTracking(true);
	m_view->setBackgroundBrush(QBrush(QColor(127, 127, 127)));
	mainLayout->addWidget(m_view);
	m_rubber_band = nullptr;
	m_hover_connection = nullptr;
	m_active_connection = nullptr;
	m_rubberband_selection = false;
	m_context_menu_enabled = true;
	m_context_menu = new QMenu(this);
	m_context_menu->addAction(new QAction(NODE_ACTION_DELETE, this));
	m_context_menu->addAction(new QAction(NODE_ACTION_CENTER, this));
	m_zoom_sub_menu = m_context_menu->addMenu(NODE_ACTION_ZOOM);
	QAction *action;
	QActionGroup actionGroupZoom(m_zoom_sub_menu);
	actionGroupZoom.setExclusive(true);
	action = new QAction(NODE_ACTION_ZOOM_10, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_25, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_50, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_75, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_90, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_100, this);
	action->setCheckable(true);
	action->setChecked(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_150, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_200, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_250, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	action = new QAction(NODE_ACTION_ZOOM_300, this);
	action->setCheckable(true);
	actionGroupZoom.addAction(action);
	m_zoom_sub_menu->addActions(actionGroupZoom.actions());

	m_context_menu->addAction(new QAction(NODE_ACTION_COLLAPSE_ALL, this));
	m_context_menu->addAction(new QAction(NODE_ACTION_EXPAND_ALL, this));
	m_context_menu->addAction(new QAction(NODE_ACTION_CENTER, this));
	m_context_menu->addAction(new QAction(NODE_ENTER_OBJECT, this));

	setMenuZoomEnabled(true);
	setMenuCollapseExpandEnabled(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_context_menu, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuItemSelected(QAction*)));
	setLayout(mainLayout);
}

QtNodeEditor::~QtNodeEditor()
{
	delete m_scene_scene;
}

//****************************************************************************/
void QtNodeEditor::setContextMenuEnabled(bool enabled)
{
	m_context_menu_enabled = enabled;
}

//****************************************************************************/
bool QtNodeEditor::isContextMenuEnabled()
{
	return m_context_menu_enabled;
}

//****************************************************************************/
void QtNodeEditor::setMenuZoomEnabled(bool enabled)
{
	m_menu_zoom_enabled = enabled;
	QAction *action = getActionFromContextMenu(NODE_ACTION_ZOOM);
	if (action)
		action->setVisible(enabled);
}

//****************************************************************************/
bool QtNodeEditor::isMenuZoomEnabled()
{
	return m_menu_zoom_enabled;
}

//****************************************************************************/
void QtNodeEditor::setMenuCollapseExpandEnabled(bool enabled)
{
	m_menu_collapse_expand_enabled = enabled;
	QAction *action = getActionFromContextMenu(NODE_ACTION_EXPAND_ALL);
	if (action)
		action->setVisible(enabled);
	action = getActionFromContextMenu(NODE_ACTION_COLLAPSE_ALL);
	if (action)
		action->setVisible(enabled);
}

//****************************************************************************/
bool QtNodeEditor::isMenuCollapseExpandEnabled()
{
	return m_menu_collapse_expand_enabled;
}

//****************************************************************************/
QGraphicsItem *QtNodeEditor::itemAtExceptActiveConnection(const QPointF &pos)
{
	auto items = m_current_scene->items(QRectF(pos - QPointF(1, 1), QSize(3, 3)));
	const bool is_active = (m_active_connection != nullptr);

	/* If there is an active connection, it is not returned as a selected item.
	 * Finalized (established) connections are returned. */
	for (QGraphicsItem *item : items) {
		if (!item->isVisible()) {
			continue;
		}

		if (!is_connection(item)) {
			return item;
		}

		if (!is_active) {
			return item;
		}
	}

	return nullptr;
}

//****************************************************************************/
QtConnection *QtNodeEditor::nodeOverConnection(QtNode *node)
{
	if (!node->hasInputs() || !node->hasOutputs()) {
		return nullptr;
	}

	/* already connected */
	if (node->input(0)->isConnected() || node->output(0)->isConnected()) {
		return nullptr;
	}

	qreal halfWidth;

	auto check_hover = [&](QtNode *nitem, QtConnection *citem)
	{
		halfWidth = 0.5 * citem->boundingRect().width();

		return (nitem->scenePos().x() > citem->scenePos().x() - halfWidth &&
		        nitem->scenePos().x() < citem->scenePos().x() + halfWidth &&
		        nitem->scenePos().y() > citem->scenePos().y() &&
		        nitem->scenePos().y() < citem->scenePos().y() + citem->boundingRect().height());
	};

	if (m_hover_connection && check_hover(node, m_hover_connection)) {
		return m_hover_connection;
	}

	QtConnection *connection;

	for (QGraphicsItem *item : m_current_scene->items()) {
		if (!is_connection(item)) {
			continue;
		}

		connection = static_cast<QtConnection *>(item);

		if (connection->isSelected()) {
			continue;
		}

		if (node->isConnectionConnectedToThisNode(connection)) {
			continue;
		}

		halfWidth = 0.5 * connection->boundingRect().width();

		if (check_hover(node, connection)) {
			return connection;
		}
	}

	if (m_hover_connection) {
		m_hover_connection->setSelected(false);
	}

	return nullptr;
}

//****************************************************************************/
bool QtNodeEditor::eventFilter(QObject *object, QEvent *event)
{
	auto mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);

	switch ((int) event->type()) {
		case QEvent::GraphicsSceneMousePress:
			mouseClickHandler(mouseEvent);
			break;
		case QEvent::GraphicsSceneMouseDoubleClick:
			mouseDoubleClickHandler(mouseEvent);
			break;
		case QEvent::GraphicsSceneMouseMove:
			mouseMoveHandler(mouseEvent);
			break;
		case QEvent::GraphicsSceneMouseRelease:
			mouseReleaseHandler(mouseEvent);
			break;
	}

	return QObject::eventFilter(object, event);
}

//****************************************************************************/
bool QtNodeEditor::mouseClickHandler(QGraphicsSceneMouseEvent *mouseEvent)
{
	switch ((int) mouseEvent->button()) {
		case Qt::LeftButton:
		{
			QGraphicsItem *item = itemAtExceptActiveConnection(mouseEvent->scenePos());
			if (!item) {
				/* Left-click on the canvas, but no item clicked, so deselect nodes and connections */
				deselectAll();
				m_rubberband_selection = true;
				m_last_mouse_position.setX(mouseEvent->lastScenePos().x());
				m_last_mouse_position.setY(mouseEvent->lastScenePos().y());
				return true;
			}

			m_rubberband_selection = false;

			/* Delegate to the node; either the node itself is clicked, one of its children or a connection */
			if (item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).isValid()) {
				int type = item->data(NODE_KEY_GRAPHIC_ITEM_TYPE).toInt();
				QtNode *node;

				if (NODE_VALUE_TYPE_CONNECTION == type) {
					/* ======================= Handle selected connection ======================= */
					QtConnection *connection = static_cast<QtConnection *>(item);
					selectConnection(connection);
				}
				else if (NODE_VALUE_TYPE_NODE == type) {
					/* ======================= The node itself is clicked ======================= */
					node = static_cast<QtNode *>(item);
					selectNode(node, mouseEvent);
				}
				else if (NODE_VALUE_TYPE_HEADER_ICON == type || NODE_VALUE_TYPE_HEADER_TITLE == type) {
					/* ======================= The header title or header icon is clicked ======================= */
					node = static_cast<QtNode *>(item->parentItem());
					selectNode(node, mouseEvent);
				}
				else {
					/* A child item of the node is clicked */
					deselectNodes();
					deselectConnections();
					node = static_cast<QtNode *>(item->parentItem());

					if (NODE_VALUE_TYPE_PORT == type) {
						/* ======================= Port is clicked ======================= */
						/* Either make a connection to another port, or create a new connection */
						QtNode *baseNode = nodeWithActiveConnection();
						if (m_active_connection == nullptr) {
							/* There is no active connection, so start one */
							node->mouseLeftClickHandler(mouseEvent, item, NODE_ACTION_BASE);
							m_active_connection = node->m_active_connection;
						}
						else if (baseNode != node) {
							/* There is an active connection and the selected
							 * port is not part of the baseNode, so try to
							 * establish a connection with the other node */
							if (node->mouseLeftClickHandler(mouseEvent, item,
							                                NODE_ACTION_TARGET,
							                                baseNode->m_active_connection))
							{
								/* The connection was established, so the active
								 * connection on the basenode can be null'd */
								baseNode->m_active_connection = nullptr;
								m_active_connection = nullptr;
							}
						}
					}
					else {
						/* Don't do anything with the node after this; it may be deleted */
						node->mouseLeftClickHandler(mouseEvent, item);
					}
				}
				return true;
			}

			break;
		}
		case Qt::RightButton:
		{
			if (m_context_menu_enabled) {
				QPoint pos;
				pos.setX(mouseEvent->lastScreenPos().x());
				pos.setY(mouseEvent->lastScreenPos().y());
				showContextMenu(pos);
			}
			else
				deselectAll();

			return true;
		}
			break;
	}

	return true;
}

//****************************************************************************/
bool QtNodeEditor::mouseDoubleClickHandler(QGraphicsSceneMouseEvent *mouseEvent)
{
	Q_UNUSED(mouseEvent);
	/* todo */
	return true;
}

//****************************************************************************/
bool QtNodeEditor::mouseMoveHandler(QGraphicsSceneMouseEvent *mouseEvent)
{
	/* If there was a rubberband selection started, update its rectangle */
	if (m_rubberband_selection && (mouseEvent->buttons() & Qt::LeftButton)) {
		rubberbandSelection(mouseEvent);
	}

	if (m_selected_nodes.size() == 1) {
		auto node = static_cast<QtNode *>(getLastSelectedNode());
		m_hover_connection = nodeOverConnection(node);

		if (m_hover_connection) {
			/* TODO: this is to highlight the conection, consider having a
			 * separate flag/colour for this. */
			m_hover_connection->setSelected(true);
			return true;
		}
	}

	for (QtNode *node : selectedNodes()) {
		node->mouseMoveHandler(mouseEvent, nullptr);
	}

	if (m_active_connection) {
		m_active_connection->updatePath(mouseEvent->scenePos());
	}

	m_hover_connection = nullptr;
	setCursor(Qt::ArrowCursor);

	return true;
}

//****************************************************************************/
bool QtNodeEditor::mouseReleaseHandler(QGraphicsSceneMouseEvent *mouseEvent)
{
	/* Determine whether a node has been dropped on a connection. */
	if (m_hover_connection) {
		splitConnectionWithNode(getLastSelectedNode());
		m_hover_connection = nullptr;
		return true;
	}

	/* Handle the rubberband selection, if applicable */
	if (!m_rubberband_selection) {
		return false;
	}

	if (mouseEvent->button() & Qt::LeftButton) {
		if (m_rubber_band) {
			qreal minX = qMin(m_last_mouse_position.x(), mouseEvent->lastScenePos().x());
			qreal maxX = qMax(m_last_mouse_position.x(), mouseEvent->lastScenePos().x());
			qreal minY = qMin(m_last_mouse_position.y(), mouseEvent->lastScenePos().y());
			qreal maxY = qMax(m_last_mouse_position.y(), mouseEvent->lastScenePos().y());
			qreal item_Min_X;
			qreal item_Max_X;
			qreal item_Min_Y;
			qreal item_Max_Y;

			/* Select the items */
			QList<QGraphicsItem*> items = m_current_scene->items();
			QtNode *node;
			QtConnection *connection;
			for (QGraphicsItem *item : items) {
				if (is_connection(item) && item->isVisible()) {
					connection = static_cast<QtConnection *>(item);

					if (connection->getBasePort() && connection->getTargetPort()) {
						item_Min_X = qMin(connection->getBasePort()->scenePos().x(), connection->getTargetPort()->scenePos().x());
						item_Max_X = qMax(connection->getBasePort()->scenePos().x(), connection->getTargetPort()->scenePos().x());
						item_Min_Y = qMin(connection->getBasePort()->scenePos().y(), connection->getTargetPort()->scenePos().y());
						item_Max_Y = qMax(connection->getBasePort()->scenePos().y(), connection->getTargetPort()->scenePos().y());

						if (item_Min_X > minX && item_Max_X < maxX &&
						    item_Min_Y > minY && item_Max_Y < maxY)
						{
							selectConnection(connection);
						}
					}
				}
				else if (is_node(item) && item->isVisible()) {
					node = static_cast<QtNode *>(item);
					item_Min_X = node->scenePos().x() - 0.5 * node->sceneBoundingRect().width() + 10;
					item_Min_Y = node->scenePos().y() - 0.5 * node->sceneBoundingRect().height() + 10;
					item_Max_X = item_Min_X + node->sceneBoundingRect().width() + 10;
					item_Max_Y = item_Min_Y + node->sceneBoundingRect().height()  + 10;

					if (item_Min_X > minX && item_Max_X < maxX &&
					    item_Min_Y > minY && item_Max_Y < maxY)
					{
						selectNode(node, nullptr);
					}
				}
			}

			m_rubber_band->hide();
		}

		m_rubberband_selection = false;
	}

	return true;
}

//****************************************************************************/
void QtNodeEditor::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete) {
		removeAllSelelected();
	}
	else if (event->key() == Qt::Key_A) {
		if (m_editor_mode == EDITOR_MODE_OBJECT) {
			m_add_node_menu->popup(QCursor::pos());
		}
	}
}

//****************************************************************************/
void QtNodeEditor::rubberbandSelection(QGraphicsSceneMouseEvent *mouseEvent)
{
	/* Mouse is pressed and moves => draw rubberband */
	qreal x = mouseEvent->lastScenePos().x();
	qreal y = mouseEvent->lastScenePos().y();

	if (!m_rubber_band) {
		m_rubber_band = new QGraphicsRectItem(m_last_mouse_position.x(), m_last_mouse_position.y(), 0.0f, 0.0f);
		m_rubber_band->setPen(QPen(Qt::darkBlue));
		QColor c(Qt::darkBlue);
		c.setAlpha(64);
		m_rubber_band->setBrush(c);
		m_current_scene->addItem(m_rubber_band);
	}

	m_rubber_band->show();
	qreal minX = qMin((qreal)m_last_mouse_position.x(), x);
	qreal maxX = qMax((qreal)m_last_mouse_position.x(), x);
	qreal minY = qMin((qreal)m_last_mouse_position.y(), y);
	qreal maxY = qMax((qreal)m_last_mouse_position.y(), y);
	m_rubber_band->setRect(minX, minY, maxX - minX, maxY - minY);
}

//****************************************************************************/
void QtNodeEditor::selectNode(QtNode *node, QGraphicsSceneMouseEvent *mouseEvent)
{
	if (mouseEvent) {
		/* pass the node itself also, because this function is also used for
		 * other purposes than selecting the node itself */
		node->mouseLeftClickHandler(mouseEvent, node);
	}

	if (mouseEvent && !ctrlPressed()) {
		deselectAll();
		m_selected_nodes.append(node);
	}
	else if (!isAlreadySelected(node)) {
		m_selected_nodes.append(node);
		/* Do not call setSelection on the node; the node is selectable, while the connection isn't */
	}

	node->setSelected(true);

	if (is_object_node(node)) {
		Q_EMIT objectNodeSelected(static_cast<ObjectNodeItem *>(node));
	}
	else {
		Q_EMIT nodeSelected(node);
	}

	toFront(node);
}

//****************************************************************************/
void QtNodeEditor::selectConnection(QtConnection *connection)
{
	if (!ctrlPressed()) {
		deselectAll();
	}

	if (!isAlreadySelected(connection)) {
		m_selected_connections.append(connection);
		connection->setSelected(true);
	}
}

//****************************************************************************/
void QtNodeEditor::deselectAll()
{
	setCursor(Qt::ArrowCursor);
	deleteAllActiveConnections();
	deselectConnections();
	deselectNodes();
}

//****************************************************************************/
void QtNodeEditor::deleteAllActiveConnections()
{
	QtNode *node;
	QList<QGraphicsItem*> items = m_current_scene->items();

	for (QGraphicsItem *item : items) {
		if (is_node(item) && item->isVisible()) {
			node = static_cast<QtNode *>(item);
			node->deleteActiveConnection();
		}
	}

	m_active_connection = nullptr;
}

//****************************************************************************/
void QtNodeEditor::deselectConnections()
{
	for (QtConnection *connection : m_selected_connections) {
		if (connection->isVisible()) {
			connection->setSelected(false);
		}
	}

	m_selected_connections.clear();
}

//****************************************************************************/
void QtNodeEditor::deselectNodes()
{
	for (QtNode *node : m_selected_nodes) {
		if (node->isVisible()) {
			node->setSelected(false);
		}
	}

	m_selected_nodes.clear();
}

//****************************************************************************/
QtNode *QtNodeEditor::nodeWithActiveConnection()
{
	if (!m_active_connection) {
		return nullptr;
	}

	return static_cast<QtNode *>(m_active_connection->getBasePort()->parentItem());
}

//****************************************************************************/
void QtNodeEditor::addNode(QtNode *node)
{
	node->setEditor(this);
	node->setScene(m_current_scene);

	m_current_scene->addItem(node);

	m_hover_connection = lastSelectedConnection();

	if (m_hover_connection != nullptr) {
		splitConnectionWithNode(node);
	}

	m_hover_connection = nullptr;
}

//****************************************************************************/
QVector<QtNode *> QtNodeEditor::getNodes() const
{
	QVector<QtNode *> nodeList;
	QtNode *node;
	QList<QGraphicsItem*> items = m_current_scene->items();
	for (QGraphicsItem *item : items) {
		if (is_node(item) && item->isVisible()) {
			node = static_cast<QtNode *>(item);
			nodeList.append(node);
		}
	}

	return nodeList;
}

//****************************************************************************/
void QtNodeEditor::removeNode(QtNode *node)
{
	if (!node) {
		return;
	}

	node->prepareDelete();

	if (is_object_node(node)) {
		Q_EMIT objectNodeRemoved(static_cast<ObjectNodeItem *>(node));
	}
	else {
		Q_EMIT nodeRemoved(node);
	}

	m_current_scene->removeItem(node);

	delete node;
}

//****************************************************************************/

using node_port_pair = std::pair<QtNode *, QtPort *>;

std::pair<node_port_pair, node_port_pair> get_base_target_pairs(QtConnection *connection, bool remove)
{
	QtPort *base_port = connection->getBasePort();
	QtPort *target_port = connection->getTargetPort();

	if (remove) {
		base_port->deleteConnection(connection);
	}

	/* make sure the connection is set in the right order */
	if (target_port->isOutputPort() && !base_port->isOutputPort()) {
		std::swap(base_port, target_port);
	}

	auto base_node = static_cast<QtNode *>(base_port->parentItem());
	auto target_node = static_cast<QtNode *>(target_port->parentItem());

	return {
		{ base_node, base_port },
		{ target_node, target_port }
	};
}

void QtNodeEditor::removeConnection(QtConnection *connection)
{
	auto pairs = get_base_target_pairs(connection, true);
	auto base = pairs.first;
	auto target = pairs.second;

	Q_EMIT connectionRemoved(base.first, base.second->getPortName(),
	                         target.first, target.second->getPortName());

	m_current_scene->removeItem(connection);
}

void QtNodeEditor::connectNodes(QtNode *from, QtPort *from_sock, QtNode *to, QtPort *to_sock)
{
	from->createActiveConnection(from_sock, from_sock->pos());
	auto connection = to_sock->createConnection(from->m_active_connection);
	m_current_scene->addItem(connection);

	from->m_active_connection = nullptr;

	Q_EMIT nodesConnected(from, from_sock->getPortName(), to, to_sock->getPortName());
}

void QtNodeEditor::splitConnectionWithNode(QtNode *node)
{
	auto connection = m_hover_connection;

	auto pairs = get_base_target_pairs(connection, true);
	auto base = pairs.first;
	auto target = pairs.second;

	/* remove connection */
	Q_EMIT connectionRemoved(base.first, base.second->getPortName(),
	                         target.first, target.second->getPortName());

	m_current_scene->removeItem(connection);

	/* connect from base port to first input port in node */
	connectNodes(base.first, base.second, node, node->input(0));

	/* connect from first output port in node to target port */
	connectNodes(node, node->output(0), target.first, target.second);
}

void QtNodeEditor::connectionEstablished(QtConnection *connection)
{
	auto pairs = get_base_target_pairs(connection, false);
	auto base = pairs.first;
	auto target = pairs.second;

	Q_EMIT nodesConnected(base.first, base.second->getPortName(),
	                      target.first, target.second->getPortName());
}

//****************************************************************************/
void QtNodeEditor::removeAllSelelected()
{
	for (QtConnection *selectedConnection : m_selected_connections) {
		removeConnection(selectedConnection);
	}

	m_selected_connections.clear();

	for (QtNode *selectedNode : m_selected_nodes) {
		removeNode(selectedNode);
	}

	m_selected_nodes.clear();
}

//****************************************************************************/
void QtNodeEditor::center()
{
	QList<QGraphicsItem*> items = m_current_scene->items();
	for (QGraphicsItem *item : items) {
		if (is_node(item)) {
			item->setPos(0, 0);
		}
	}
}

//****************************************************************************/
void QtNodeEditor::clear()
{
	m_current_scene->clear(); /* removes + deletes all items in the scene */
}

//****************************************************************************/
QtNode *QtNodeEditor::getLastSelectedNode() const
{
	if (m_selected_nodes.empty()) {
		return nullptr;
	}

	return m_selected_nodes.back();
}

//****************************************************************************/
const QVector<QtNode *> &QtNodeEditor::selectedNodes() const
{
	return m_selected_nodes;
}

//****************************************************************************/
QtConnection *QtNodeEditor::lastSelectedConnection() const
{
	if (m_selected_connections.empty()) {
		return nullptr;
	}

	return m_selected_connections.back();
}

//****************************************************************************/
const QVector<QtConnection *> &QtNodeEditor::selectedConnections() const
{
	return m_selected_connections;
}

//****************************************************************************/
void QtNodeEditor::toFront(QtNode *node)
{
	if (!node) {
		return;
	}

	QList<QGraphicsItem *> items = m_current_scene->items();

	/* First set the node in front of all other nodes */
	for (QGraphicsItem *item : items) {
		if (node != item && is_node(item) && item->isVisible()) {
			item->stackBefore(node);
		}
	}

	/* Put the connections of the node in front of the node and the other connections behind the node */
	for (QGraphicsItem *item : items) {
		if (!is_node(item)) {
			continue;
		}

		QtConnection *connection = static_cast<QtConnection *>(item);

		if (node->isConnectionConnectedToThisNode(connection)) {
			node->stackBefore(item);
		}
		else {
			item->stackBefore(node);
		}
	}
}

//****************************************************************************/
void QtNodeEditor::toBack(QtNode *node)
{
	if (!node)
		return;

	QList<QGraphicsItem*> items = m_current_scene->items();

	/* Set all other nodes in front of this node */
	for (QGraphicsItem *item : items) {
		if (node != item && is_node(item) && item->isVisible()) {
			node->stackBefore(item);
		}
	}
}

//****************************************************************************/
void QtNodeEditor::setZoom(qreal zoom)
{
	m_view->scale(zoom, zoom);
}

//****************************************************************************/
bool QtNodeEditor::ctrlPressed()
{
	return (QGuiApplication::keyboardModifiers() & Qt::ControlModifier);
}

//****************************************************************************/
bool QtNodeEditor::isAlreadySelected(QtNode *node)
{
	auto iter = std::find(m_selected_nodes.begin(),
	                      m_selected_nodes.end(),
	                      node);

	return (iter != m_selected_nodes.end());
}

//****************************************************************************/
bool QtNodeEditor::isAlreadySelected(QtConnection *connection)
{
	auto iter = std::find(m_selected_connections.begin(),
	                      m_selected_connections.end(),
	                      connection);

	return (iter != m_selected_connections.end());
}

//****************************************************************************/
void QtNodeEditor::showContextMenu(const QPoint &pos)
{
	if (!m_context_menu) {
		return;
	}

	QList<QAction *> actions = m_context_menu->actions();
	const bool nodesSelected = !m_selected_nodes.isEmpty();
	const bool itemsSelected = nodesSelected || !m_selected_connections.isEmpty();

	QFont font;
	/* Italic when no selected items available */
	font.setItalic(!itemsSelected);

	for (auto &action : actions) {
		if (action->text() == NODE_ACTION_DELETE) {
			action->setFont(font);
			action->setEnabled(itemsSelected);
		}

		if (action->text() == NODE_ENTER_OBJECT) {
			action->setFont(font);
			action->setEnabled(nodesSelected);
		}
	}

	m_context_menu->popup(pos);
}

//****************************************************************************/
QAction *QtNodeEditor::getActionFromContextMenu(const QString &actionText)
{
	if (!m_context_menu) {
		return nullptr;
	}

	QList<QAction*> actions = m_context_menu->actions();
	for (QAction *action : actions) {
		if (action->text() == actionText) {
			return action;
		}
	}

	return nullptr;
}

//****************************************************************************/
void QtNodeEditor::setZoomForAction(qreal zoom, QAction *action)
{
	setZoom(zoom);
	resetZoomSubmenu();
	action->setChecked(true);
}

//****************************************************************************/
void QtNodeEditor::resetZoomSubmenu()
{
	QList<QAction*> actions = m_zoom_sub_menu->actions();
	for (QAction *action : actions)
		action->setChecked(false);
}

//****************************************************************************/
void QtNodeEditor::contextMenuItemSelected(QAction *action)
{
	/* ---------------- Delete action ---------------- */
	if (action->text() == NODE_ACTION_DELETE) {
		/* Delete the selected nodes and connections from the scene */
		removeAllSelelected();
		return;
	}

	/* ---------------- Center action ---------------- */
	if (action->text() == NODE_ACTION_CENTER) {
		/* Center all nodes */
		center();
		return;
	}

	/* ---------------- Zoom action ---------------- */
	if (action->text() == NODE_ACTION_ZOOM_10) {
		setZoomForAction(0.1f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_25) {
		setZoomForAction(0.25f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_50) {
		setZoomForAction(0.5f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_75) {
		setZoomForAction(0.75f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_90) {
		setZoomForAction(0.9f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_100) {
		setZoomForAction(1.0f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_150) {
		setZoomForAction(1.5f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_200) {
		setZoomForAction(2.0f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_250) {
		setZoomForAction(2.5f, action);
		return;
	}

	if (action->text() == NODE_ACTION_ZOOM_300) {
		setZoomForAction(3.0f, action);
		return;
	}

	/* ---------------- Collapse action ---------------- */
	if (action->text() == NODE_ACTION_COLLAPSE_ALL) {
		QtNode *node;
		QList<QGraphicsItem*> items = m_current_scene->items();

		for (QGraphicsItem *item : items) {
			if (is_node(item) && item->isVisible()) {
				node = static_cast<QtNode*>(item);
				node->collapse();
			}
		}

		return;
	}

	/* ---------------- Expand action ---------------- */
	if (action->text() == NODE_ACTION_EXPAND_ALL) {
		QtNode *node;
		QList<QGraphicsItem*> items = m_current_scene->items();

		for (QGraphicsItem *item : items) {
			if (is_node(item) && item->isVisible()) {
				node = static_cast<QtNode *>(item);
				node->expand();
			}
		}

		return;
	}

	/* ---------------- Enter object action ---------------- */
	if (action->text() == NODE_ENTER_OBJECT) {
		auto node = static_cast<ObjectNodeItem *>(getLastSelectedNode());
		m_current_scene = node->nodeScene();
		m_current_scene->installEventFilter(this);
		m_editor_mode = EDITOR_MODE_OBJECT;
		m_view->setScene(m_current_scene);

		action->setText(NODE_EXIT_OBJECT);

		return;
	}

	/* ---------------- Exit object action ---------------- */
	if (action->text() == NODE_EXIT_OBJECT) {
		m_current_scene = m_scene_scene;
		m_view->setScene(m_current_scene);

		action->setText(NODE_ENTER_OBJECT);

		return;
	}
}

/* ************************************************************************** */

NodeView::NodeView(QWidget *parent)
    : QGraphicsView(parent)
{
	setDragMode(QGraphicsView::ScrollHandDrag);
}

NodeView::NodeView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
	setDragMode(QGraphicsView::ScrollHandDrag);
}

void NodeView::wheelEvent(QWheelEvent *event)
{
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	const auto factor = 1.15f;
	const auto zoom = ((event->delta() > 0) ? factor : 1.0f / factor);

	this->scale(zoom, zoom);
}
