#include <cqde/types/UndoRedoQueue-inl.hpp>

#include <cqde/alias.hpp>

#include <json/value.h>

#include <map>


namespace cqde::types
{

template class
UndoRedoQueue <Json::Value>;

template class
UndoRedoQueue <std::map <PackageId, Json::Value>>;

}
