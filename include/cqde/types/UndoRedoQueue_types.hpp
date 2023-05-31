#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/UndoRedoQueue.hpp>

#include <json/value.h>

#include <map>


namespace cqde::types
{

extern template class
UndoRedoQueue <Json::Value>;

extern template class
UndoRedoQueue <std::map <PackageId, Json::Value>>;

}
