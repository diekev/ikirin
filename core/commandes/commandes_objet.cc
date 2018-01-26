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
 * The Original Code is Copyright (C) 2016 Kévin Dietrich.
 * All rights reserved.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

#include "commandes_objet.h"

#include <kamikaze/context.h>
#include <kamikaze/noeud.h>
#include <kamikaze/operateur.h>
#include <kamikaze/primitive.h>

#include "graphs/object_graph.h"
#include "operateurs/operateurs_standards.h"

#include "object.h"
#include "scene.h"

/* *************************** add object command *************************** */

void AddObjectCmd::execute(Main */*main*/, const Context &context, const std::string &metadonnee)
{
	m_scene = context.scene;

	m_object = new Object(context);
	m_object->name(metadonnee);

	assert(m_scene != nullptr);
	m_scene->addObject(m_object);
}

void AddObjectCmd::defait()
{
	/* TODO */
}

void AddObjectCmd::refait()
{
	/* TODO */
}

/* **************************** add node command **************************** */

void AddNodeCmd::execute(Main */*main*/, const Context &context, const std::string &metadonnee)
{
	m_scene = context.scene;
	auto scene_node = m_scene->active_node();

	if (scene_node == nullptr) {
		return;
	}

	m_object = static_cast<Object *>(scene_node);

	assert(m_object != nullptr);

	auto noeud = new Noeud();
	noeud->nom(metadonnee);

	auto operateur = (*context.usine_operateur)(metadonnee, noeud, context);
	static_cast<void>(operateur);

	noeud->synchronise_donnees();

	m_object->ajoute_noeud(noeud);

	m_scene->notify_listeners(event_type::node | event_type::added);
}

void AddNodeCmd::defait()
{
	/* TODO */
}

void AddNodeCmd::refait()
{
	/* TODO */
}

/* **************************** add torus command **************************** */

void AddPresetObjectCmd::execute(Main */*main*/, const Context &context, const std::string &metadonnee)
{
	m_scene = context.scene;

	if (context.eval_ctx->edit_mode) {
		auto scene_node = m_scene->active_node();

		/* Sanity check. */
		if (scene_node == nullptr) {
			return;
		}

		m_object = static_cast<Object *>(scene_node);
	}
	else {
		m_object = new Object(context);
		m_object->name(metadonnee);
	}

	assert(m_object != nullptr);

	auto noeud = new Noeud();
	noeud->posx(-300);
	noeud->posy(-100);

	(*context.usine_operateur)(metadonnee, noeud, context);

	noeud->synchronise_donnees();

	m_object->ajoute_noeud(noeud);

	auto graph = m_object->graph();
	graph->ajoute_selection(noeud);
	graph->connecte(noeud->sortie(0), graph->sortie()->entree(0));

	if (!context.eval_ctx->edit_mode) {
		m_scene->addObject(m_object);
		m_scene->evalObjectDag(context, m_object);
	}
	else {
		m_scene->notify_listeners(event_type::node | event_type::added);
	}
}

void AddPresetObjectCmd::defait()
{
	/* TODO */
}

void AddPresetObjectCmd::refait()
{
	/* TODO */
}

/* ************************************************************************** */

class CommandeEntreObjet : public Commande {
public:
	CommandeEntreObjet() = default;
	~CommandeEntreObjet() = default;

	void execute(Main */*main*/, const Context &context, const std::string &/*metadonnee*/) override
	{
		context.eval_ctx->edit_mode = true;
		context.scene->notify_listeners(event_type::object | event_type::selected);
	}

	void defait() override {}
	void refait() override {}
};

/* ************************************************************************** */

class CommandeSorsObjet : public Commande {
public:
	CommandeSorsObjet() = default;
	~CommandeSorsObjet() = default;

	void execute(Main */*main*/, const Context &context, const std::string &/*metadonnee*/) override
	{
		context.eval_ctx->edit_mode = false;
		context.scene->notify_listeners(event_type::object | event_type::selected);
	}

	void defait() override {}
	void refait() override {}
};

/* ************************************************************************** */

void enregistre_commandes_objet(CommandFactory *usine)
{
	ENREGISTRE_COMMANDE(usine, "ajouter_objet", AddObjectCmd);
	ENREGISTRE_COMMANDE(usine, "ajouter_noeud", AddNodeCmd);
	ENREGISTRE_COMMANDE(usine, "ajouter_prereglage", AddPresetObjectCmd);
	ENREGISTRE_COMMANDE(usine, "objet.entre", CommandeEntreObjet);
	ENREGISTRE_COMMANDE(usine, "objet.sors", CommandeSorsObjet);
}
