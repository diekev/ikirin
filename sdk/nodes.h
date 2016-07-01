/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2015 Kévin Dietrich.
 * All rights reserved.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "models.h"

class InputSocket;
class Node;
class NodeFactory;
class ParamCallback;
class Primitive;
class PrimitiveCache;

extern "C" {

/**
 * @brief new_kamikaze_node API for registering a new node from a plugin.
 *                          There is no limit to the number of nodes to register
 *                          from a single call to this function.
 * @param factory The factory in which to register the node(s).
 */
void new_kamikaze_node(NodeFactory *factory);

}

/**
 * Macro to help registering nodes.
 */
#define REGISTER_NODE(category, name, type) \
	factory->registerType(category, name, []() -> Node* { return new type; })

struct OutputSocket {
	Node *parent = nullptr;
	std::vector<InputSocket *> links{};
	std::string name = "";
	Primitive *prim = nullptr;

	explicit OutputSocket(const std::string &sname)
	    : parent(nullptr)
	    , name(sname)
	{}
};

struct InputSocket {
	Node *parent = nullptr;
	OutputSocket *link = nullptr;
	std::string name = "";
	Primitive *prim = nullptr;

	explicit InputSocket(const std::string &sname)
	    : parent(nullptr)
	    , link(nullptr)
	    , name(sname)
	{}
};

/* ********************************** */

struct NodeState {
	std::vector<InputSocket *> inputs;
	std::vector<OutputSocket *> outputs;
};

using NodeWatcher = Watcher<NodeState>;

class Node : public Watched<NodeWatcher> {
protected:
	std::vector<InputSocket *> m_inputs;
	std::vector<OutputSocket *> m_outputs;
	std::string m_name;
	PrimitiveCache *m_cache;

public:
	explicit Node(const std::string &name);
	Node(const Node &other) = default;

	virtual ~Node();

	/**
	 * Return the name of this node.
	 */
	const std::string &name() const noexcept;

	/**
	 * Add an input to the node.
	 */
	void addInput(const std::string &sname);

	/**
	 * Add an output to the node.
	 */
	void addOutput(const std::string &sname);

	/**
	 * Return the input at the given index.
	 */
	InputSocket *input(int index);

	/**
	 * Return the input with the given name.
	 */
	InputSocket *input(const std::string &name);

	/**
	 * Return the output at the given index.
	 */
	OutputSocket *output(int index);

	/**
	 * Return the output with the given name.
	 */
	OutputSocket *output(const std::string &name);

	/**
	 * Return the vector of inputs of this node.
	 */
	std::vector<InputSocket *> inputs() const noexcept;

	/**
	 * Return the vector of outputs of this node.
	 */
	std::vector<OutputSocket *> outputs() const noexcept;

	/**
	 * Return whether or not this node is linked.
	 */
	bool isLinked() const;

	/**
	 * Return whether or not this node has a linked input.
	 */
	bool hasLinkedInput() const;

	/**
	 * Return whether or not this node has a linked output.
	 */
	bool hasLinkedOutput() const;

	/**
	 * Process the node.
	 */
	virtual void process() = 0;

	/**
	 * Get the primitive at the given input name.
	 */
	Primitive *getInputPrimitive(const std::string &name);

	/**
	 * Set the primitive at the given output name.
	 */
	void setOutputPrimitive(const std::string &name, Primitive *prim);

	/**
	 * Set the primitive cache.
	 */
	void setPrimitiveCache(PrimitiveCache *cache);

	NodeState getState() const
	{
		NodeState s;
		s.inputs = this->m_inputs;
		s.outputs = this->m_outputs;
		return s;
	}
};

class NodeFactory final {
public:
	typedef Node *(*factory_func)(void);

	void registerType(const std::string &category, const std::string &name, factory_func func);

	Node *operator()(const std::string &name);

	size_t numEntries() const;

	std::vector<std::string> keys(const std::string &category) const;

	std::vector<std::string> categories() const;

private:
	std::unordered_map<std::string, factory_func> m_map;
	std::unordered_map<std::string, std::vector<std::string>> m_cat_map;
};
