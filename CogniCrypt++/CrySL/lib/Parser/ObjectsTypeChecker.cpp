#include <CrySLTypechecker.h>
#include <PositionHelper.h>
#include <TypeParser.h>

namespace CCPP {
using namespace std;

bool typecheck(
    std::unordered_map<std::string, std::shared_ptr<Type>> &DefinedObjects,
    CrySLParser::ObjectDeclContext *decl, const std::string &filename) {
  auto name = decl->Ident()->getText();
  if (DefinedObjects.count(name)) {
    cerr << Position(decl, filename) << ": An object with the name '" << name
         << "' is already defined" << endl;
    return false;
  } else {
    DefinedObjects[name] =
        getOrCreateType(decl->typeName(), decl->constModifier != nullptr);
    return true;
  }
}

bool CrySLTypechecker::CrySLSpec::typecheck(CrySLParser::ObjectsContext *objs) {
  bool succ = true;
  for (auto decl : objs->objectDecl()) {
    succ &= ::CCPP::typecheck(this->DefinedObjects, decl, filename);
  }
  return succ;
}
} // namespace CCPP