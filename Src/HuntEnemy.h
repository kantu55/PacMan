#ifndef HUNTENEMY_H_INCLUDED
#define HUNTENEMY_H_INCLUDED
#include "Enemy.h"

class HuntEnemy : public EnemyActor 
{
public:
	HuntEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0));
	virtual ~HuntEnemy() = default;
	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);
};
using HuntEnemyPtr = std::shared_ptr<HuntEnemy>;

#endif // !HUNTENEMY_H_INCLUDED

