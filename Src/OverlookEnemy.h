#ifndef OVERLOOKENEMY_H_INCLUDED
#define OVERLOOKENEMY_H_INCLUDED
#include "Enemy.h"

class OverlookEnemy : public EnemyActor
{
public:
	OverlookEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0));
	virtual ~OverlookEnemy() = default;
	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);
private:
	
};
using OverlookEnemyPtr = std::shared_ptr<OverlookEnemy>;

#endif // !OVERLOOKENEMY_H_INCLUDED
