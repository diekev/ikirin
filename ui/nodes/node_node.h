/****************************************************************************
**
* *Copyright(C) 2014
**
* *This file is generated by the Magus toolkit
**
* *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* *"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* *LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* *A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* *OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* *SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
* *LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* *DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* *THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* *(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#pragma once

#include <QPen>

#include "node_constants.h"
#include "node_port.h"

class Node;

static constexpr auto NODE_ACTION_BASE = 1;
static constexpr auto NODE_ACTION_TARGET = 2;

class QtPort;
class QtConnection;
class QtNodeEditor;

/****************************************************************************
	QtNode represents a Node-class which is visualised in a QGraphicsScene.
	***************************************************************************/
class QtNode : public QGraphicsPathItem {
	Node *m_data;

	bool m_auto_size = true;
	qreal m_icon_size;
	qreal m_normalized_width;
	qreal m_width;
	qreal m_normalized_body_height;
	qreal m_header_height;
	qreal m_body_height;
	QGraphicsPathItem *m_body;
	QBrush m_header_brush;
	QPen m_pen;
	QtNodeEditor *m_editor;
	QGraphicsScene *m_scene;
	QString m_title;
	QGraphicsTextItem *m_title_label;
	QFont m_font_header;
	Alignment m_title_alignment;

	QVector<QtPort *> m_port_list;
	QVector<QtPort *> m_input_ports;
	QVector<QtPort *> m_output_ports;

	QColor m_port_name_color;
	QGraphicsItem *m_original_parent;

public:
	QtConnection *m_active_connection;

	QtNode(const QString &title, QGraphicsItem *parent = nullptr);
	virtual ~QtNode();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	void setNode(Node *node);

	Node *getNode() const;

	/* Set the editor; this is called(by the editor) as soon as a node is added
	 * to the scene. Do not use this(only for internal use) */
	void setEditor(QtNodeEditor *editor);

	/* Set the scene; this is called(by the editor) as soon as a node is added
	 * to the scene. Do not use this (only for internal use) */
	void setScene(QGraphicsScene *scene);

	/* Called from the QNodeEditor; used for adding a node to a compound */
	/* Overridden */
	void setParentItem(QGraphicsItem *parent);

	/* Called from the QNodeEditor; used for removing a node from a compound */
	void restoreOriginalParentItem();

	/* Set the color of the title in the header */
	void setTitleColor(const QColor &color);

	/* Align the text of the header (default is center) */
	void alignTitle(Alignment alignment);

	/* Create a port
	 * portId:          Identification (handle) of the port.
	 * portName:        The name of the port. This is also the text displayed.
	 * type:            Define the type of port. By means of the portType it is also possible to define whether it is
	 *                  possible to connect this port to another port (of another or the same type). By default, 2 prefab
	 *                  portTypes exist, a QtInputPortType and a QtOutputPortType. It is also possible to create your own
	 *                  port type.
	 * portColour:      The color of a port.
	 * portShape:       A port can be shaped circular or as a square.
	 * alignement:      A port can be positioned left, right or in the middle of a node.
	 * connectionColor: If connecting is started from this port, it gets the color defined by 'connectionColor'.
	 *                  The created port is in that case the 'base port' of the connection.
	 */
	QtPort *createPort(const QString &portName,
	                   int type,
	                   QColor portColour,
	                   Alignment alignement,
	                   QColor connectionColor = Qt::black);

	/* Returns true, if a given connection is connected to a port of 'this' node */
	bool isConnectionConnectedToThisNode(QtConnection *connection);

	/* Create an active connection that must be connected to another node (using
	 * the mouse).
	 * port = The base port to which the connection is attached; this is also
	 *        the start position.
	 * pos = End position */
	void createActiveConnection(QtPort *port, QPointF pos);

	/* Delete the connection that is currently active */
	void deleteActiveConnection();

	/* Delete all connections, connected to this node */
	void deleteAllConnections();

	/* Called from the QNodeEditor when either a left mouse click on the QtNode
	 * has taken place, or on one of its child QGraphicsItems. The
	 * mouseLeftClickHandler function performs default behaviour, but it is
	 * possible to overload this function. */
	virtual bool mouseLeftClickHandler(QGraphicsSceneMouseEvent *mouseEvent,
	                                   QGraphicsItem *item,
	                                   unsigned int action = 0,
	                                   QtConnection *activeConnection = nullptr);

	/* Collapse the node (and hide the ports) */
	void collapse();

	/* Make all ports visible and restore the endpoint of the connection */
	void expand();

	bool hasInputs() const;
	bool hasOutputs() const;

	QtPort *input(int index) const;
	QtPort *output(int index) const;

	QVector<QtPort *> getPorts() const;

protected:
	void setTitlePosition();
	void adjustWidthForTitle();
	void setPortAlignedPos(QtPort *port, qreal height);
	void redraw();
};
