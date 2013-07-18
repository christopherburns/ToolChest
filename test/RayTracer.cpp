#include "../toolchest/ToolChest.h"

using namespace ToolChest;


////////////////
// Components //
////////////////

/* There are several ways to organize the storage of component
  instances. If removals are uncommon, an array or vector indexed
  by a component instance id might be appropriate. If removals are
  common, then a HashMap would be better.

  Components do not know their own ID, only the entities that
  reference them.


*/

template <class T> ComponentStore

class AbstractComponent
{
  virtual void Remove() = 0;
};

template <class T> class HashedComponent : public AbstractComponent
{
private:
  static AtomicUniqueID _guidCounter;

  const UniqueID _guid;

  // maps component id to the component itself
  static Mutable::HashMap<UniqueID, T> _componentBag;
  static T& CreateNewComponent(UniqueID& id);

  friend class Context;

public:
  HashedComponent() : _guid(_guidCounter++) { }
  UniqueID GetID() const { return _guid; }
};

enum ComponentType
{
  REFERENCE_FRAME,
  BOUNDS3D,
  etc.
};

struct ReferenceFrame : public HashedComponent<ReferenceFrame>
{
  Matrix4x4 xForm;
  static ComponentTypeID TypeID = REFERENCE_FRAME;
};

struct Bounds3D : public HashedComponent<Bounds3D>
{
  Vector3f ur, ll;
  static const ComponentTypeID TypeID = BOUNDS3D;
};

struct TriangleList : public HashedComponent<TriangleList>
{ Immutable::Array<Vector3i> triangles; };

struct Vertex
{ Vector4f position, tangent, bitangent, texCoord; };

struct VertexList : public HashedComponent<VertexList>
{ Immutable::Array<Vertex> vertices; };

struct TopologyList : public HashedComponent<TopologyList>
{ Immutable::Array<int32> topology; };

struct RaySegment : public HashedComponent<RaySegment>
{ Vector3f direction, origin; Vector2f tRange; };

struct Illuminaire : public HashedComponent<Illuminaire>
{ Vector3f power; };

struct Phong : public HashedComponent<Phong>
{
  Vector3f diffuse, specular;
  float specularExp;
};



int main()
{

	return 0;
}
