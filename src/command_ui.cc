// rTorrent - BitTorrent client
// Copyright (C) 2005-2006, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#include "config.h"

#include <rak/functional_fun.h>
#include <sigc++/bind.h>

// #include "core/download_list.h"
#include "core/manager.h"
#include "core/view_manager.h"
#include "utils/command_slot.h"
#include "utils/parse.h"

#include "globals.h"
#include "control.h"
#include "command_helpers.h"

typedef void (core::ViewManager::*view_filter_slot)(const std::string&, const core::ViewManager::sort_args&);

void
apply_view_filter(view_filter_slot viewFilterSlot, const torrent::Object& rawArgs) {
  const torrent::Object::list_type& args = rawArgs.as_list();

  if (args.size() < 1)
    throw torrent::input_error("Too few arguments.");

  const std::string& name = args.front().as_string();
  
  if (name.empty())
    throw torrent::input_error("First argument must be a string.");

  core::ViewManager::filter_args filterArgs;

  for (torrent::Object::list_type::const_iterator itr = ++args.begin(), last = args.end(); itr != last; itr++)
    filterArgs.push_back(itr->as_string());

  (control->view_manager()->*viewFilterSlot)(name, filterArgs);
}

void
apply_view_sort(const torrent::Object& rawArgs) {
  const torrent::Object::list_type& args = rawArgs.as_list();

  if (args.size() <= 0 || args.size() > 2)
    throw torrent::input_error("Wrong argument count.");

  const std::string& name = args.front().as_string();

  if (name.empty())
    throw torrent::input_error("First argument must be a string.");

  int32_t value = 0;

  if (args.size() == 2)
    value = utils::convert_to_value(args.back());

  control->view_manager()->sort(name, value);
}

void
initialize_command_ui() {
  utils::VariableMap* variables = control->variable();
//   core::DownloadList* downloadList = control->core()->download_list();

  ADD_COMMAND_SLOT("view_filter",       call_list, rak::bind_ptr_fn(&apply_view_filter, &core::ViewManager::set_filter));
  ADD_COMMAND_SLOT("view_filter_on",    call_list, rak::bind_ptr_fn(&apply_view_filter, &core::ViewManager::set_filter_on));

  ADD_COMMAND_SLOT("view_sort",         call_list, rak::ptr_fn(&apply_view_sort));
  ADD_COMMAND_SLOT("view_sort_new",     call_list, rak::bind_ptr_fn(&apply_view_filter, &core::ViewManager::set_sort_new));
  ADD_COMMAND_SLOT("view_sort_current", call_list, rak::bind_ptr_fn(&apply_view_filter, &core::ViewManager::set_sort_current));
}