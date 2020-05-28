#ifndef GATEKEEPERENEMY_H_INCLUDED
#define GATEKEEPERENEMY_H_INCLUDED
#include "Enemy.h"

class GateKeeperEnemy : public EnemyActor
{
public:
	GateKeeperEnemy(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3& rot = glm::vec3(0));
	virtual ~GateKeeperEnemy() = default;
	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);

	float keeperDirection; // ŠÄŽ‹‚·‚éŒü‚«
};
using GateKeeperEnemyPtr = std::shared_ptr<GateKeeperEnemy>;

#endif // !GATEKEEPERENEMY_H_INCLUDED
