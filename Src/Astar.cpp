#include"Astar.h"
#include<stdio.h>

#define ARRAY_NUM(a) (sizeof(a)/sizeof(a[0]))
#define NODE_MAX 1000

/*
 ���̃m�[�h���쐬
*/
Astar::NODE *Astar::CreateNode(int x, int z, int cost)
{
	static Astar::NODE n[NODE_MAX];
	n[index].x = x;
	n[index].z = z;
	n[index].cost = cost;
	return &n[index++];
}

/*
�X�^�[�g�m�[�h����n�܂ł̍ŏ��R�X�g��Ԃ�
*/
int Astar::G(NODE *s, NODE *n)
{
	return n->cost;
}

/*
�S�[���m�[�h����n�܂ł̍ŏ��R�X�g��Ԃ�
*/
int Astar::H(NODE *e, NODE *n)
{
	return 0;
}

/*
*�m�[�h�𒲂ׂ�
*
*@param open  ��  �v�Z���̃m�[�h���i�[���Ă������߂̃I�[�v�����X�g
*@param close  ��  �v�Z�ς݂̃m�[�h���i�[���邽�߂̃N���[�Y���X�g
*@param s  ��  �X�^�[�g�m�[�h
*@param e  ��  �S�[���m�[�h
*@param n  ��  ���݂̃m�[�h
*@param m  ��  n�̃m�[�h�ɗאڂ��Ă���m�[�h
*/
void Astar::SearchNode(LIST *open, LIST *close, NODE *s,
	NODE *e, NODE *n, NODE *m)
{
	int inOpen = -1; // open���X�g�p�̃`�F�b�N�ϐ�
	int inClose = -1; // close���X�g�p�̃`�F�b�N�ϐ�
	int i; // for���p�̕ϐ�
	// G + H + (n����m�܂ňړ�����R�X�g)
	int fdmCost = G(s, n) + H(e, m) + 1;
	// G + H
	int fsmCost = G(s, m) + H(e, m);

	// m��open���X�g�Ɋ܂܂�Ă��邩
	for (i = 0; i < open->index; i++)
	{
		if (open->node[i] != NULL &&
			m->x == open->node[i]->x &&
			m->z == open->node[i]->z)
		{
			inOpen = i;
			break;
		}
	}

	// m��close���X�g�Ɋ܂܂�Ă��邩
	for (i = 0; i < close->index; i++)
	{
		if (close->node[i] != NULL &&
			m->x == close->node[i]->x &&
			m->z == close->node[i]->z)
		{
			inClose = i;
			break;
		}
	}

	// m��open���X�g�ɂ�close���X�g�ɂ��܂܂�Ă��Ȃ��ꍇ
	if (inOpen == -1 && inClose == -1)
	{
		m->parent = n; // m�̐e��n�Ƃ��Ċi�[
		open->node[open->index++] = m; // m��open���X�g�ɒǉ�
	}

	// m��open���X�g�Ɋ܂܂�Ă���ꍇ
	if (inOpen > -1)
	{
		if (fdmCost < fsmCost)
		{
			m->parent = n; // m�̐e��n�Ƃ��Ċi�[
		}
	}

	// m��close���X�g�Ɋ܂܂�Ă���ꍇ
	if (inClose > -1)
	{
		if (fdmCost < fsmCost)
		{
			m->parent = n; // m�̐e��n�Ƃ��Ċi�[
			open->node[open->index++] = m; // m��open���X�g�ɒǉ�
			close->node[inClose] = NULL; // close���X�g����ɂ���
		}
	}
}

Astar::NODE* Astar::GetMinCost(Astar::NODE *n)
{
	for (int x = 0; x < open.index; x++)
	{
		if (open.node[x] != NULL)
		{
			int cost = open.node[x]->cost;
			if (n == NULL || n->cost > cost)
			{
				// �m�[�h�̒��ň�ԍŏ��̃R�X�g�𓾂�
				n = open.node[x];
				open.node[x] = NULL;
			}
		}
	}
	return n;
}

glm::vec3  Astar::CreateRoute(Astar::NODE *n)
{
	glm::vec3 p;
	costError = false;
	if (n->cost >= n->parent->cost)
	{
		n = n->parent;
		p = glm::vec3(n->x, 0, n->z);
	}
	// �e�m�[�h�̃R�X�g���傫�������ꍇ�̗�O����
	else if (n->cost < n->parent->cost)
	{
		printf("�R�X�g�G���[\n");
		// �e�m�[�h�̋�ɂ���
		n->parent = NULL;
		costError = true;
	}
	return p;
}