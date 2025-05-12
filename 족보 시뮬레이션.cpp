#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<ctime>
#include<vector>
#include<algorithm>

#define RANK_CNT 13
#define RANK_LEN 3
#define SHAPE_CNT 4
#define SHAPE_LEN 10
#define CARD_CNT 60
#define JOKER_CNT 8
#define NOTES 1500
#define LINE 7
#define HAND_CNT 9
#define SIMUL 1000000

using namespace std;

const char* rank_type[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"}; // ��ũ ���� ���� 
const char* shape_type[SHAPE_CNT] = {"diamond", "spade", "heart", "clover"}; // ���� ���� ���� 

struct card{
	char rank[RANK_LEN]; // ��ũ
	char shape[SHAPE_LEN]; // ����
	int durable; // ������ 
	bool isSpecial; // Ư��ī�� ���� 
};

unsigned int familyFreq[HAND_CNT]; // �� ������ ���� Ƚ��
unsigned int topFamilyCount[HAND_CNT]; // �ش� ������ �ֻ��� ������ ������ Ƚ��
unsigned int totalFamilyFreq[HAND_CNT];
unsigned int totalTopFamilyCount[HAND_CNT];
vector <card> deck; // deck�� �� ī��� 
vector <card> grave; // ���� �� ī���
vector <int> note_lines; // ��Ʈ�� ������ ������ index 

void initDeck() {
    deck.clear();
    grave.clear();

	// �Ϲ� 52�� deck�� �߰� 
    for (int s = 0; s < SHAPE_CNT; s++) {
        for (int r = 0; r < RANK_CNT; r++) {
            card c;
            
            strcpy(c.rank, rank_type[r]);
            strcpy(c.shape, shape_type[s]);
            c.durable = 20;
            c.isSpecial = false;
            deck.push_back(c);
        }
    }

    // ��Ŀ 8�� deck�� �߰� 
    for (int r = 0; r < JOKER_CNT; r++) {
        card c;
        
        strcpy(c.rank, rank_type[r]);
        strcpy(c.shape, "diamond");
        c.durable = 20;
        c.isSpecial = true;
        deck.push_back(c);
    }
}

void initHand()	{
	for(int h = 0; h < HAND_CNT; h++)	{
		familyFreq[h] = 0;
		topFamilyCount[h] = 0;
	}
}

void initNote() {
    note_lines.clear();
    
    for (int i = 0; i < LINE; i++) {
        int count = 1500 / LINE + (i < (1500 % LINE) ? 1 : 0);
        for (int j = 0; j < count; j++) {
            note_lines.push_back(i);
        }
    }

    random_shuffle(note_lines.begin(), note_lines.end()); // ���� random�ϰ� ���� 
}

card draw_card() {
    if (deck.empty()) {
        for (int i = 0; i < grave.size(); i++) {
            grave[i].durable = 20;
            deck.push_back(grave[i]);
        }
        grave.clear();
    } // deck�� �� ��� grave���� ī�� revive 

    int idx = rand() % deck.size(); // ī�� draw 
    card newCard = deck[idx];
    deck.erase(deck.begin() + idx); // �ش� ī�� ������ ���� 
    
    return newCard;
}

void check_hand(card field[7]) {
	int bestHand = -1; 
    int rank_count[RANK_CNT] = {0};
    int shape_count[SHAPE_CNT] = {0};
    bool rank_exist[RANK_CNT] = {false};

    for (int i = 0; i < LINE; i++) {
        // ��ũ index
        int rank = 0;
        for (int r = 0; r < RANK_CNT; r++) {
            if (strcmp(field[i].rank, rank_type[r]) == 0) {
                rank = r;
                break;
            }
        }
        rank_count[rank]++;
        rank_exist[rank] = true;

        // ���� index 
        for (int s = 0; s < SHAPE_CNT; s++) {
            if (strcmp(field[i].shape, shape_type[s]) == 0) {
                shape_count[s]++;
                break;
            }
        }
    }

    // ��Ʈ����Ʈ/��Ʈ����Ʈ �÷��� Ž��
    for (int start = 0; start <= 8; start++) {
        bool straight = true;
        bool straightFlash = false;
        for (int j = 0; j < 5; j++) {
            if (!rank_exist[start + j]) {
                straight = false;
                break;
            }
        }

        if (straight) {
            familyFreq[4]++;
            if(bestHand < 4)	bestHand = 4; // ��Ʈ����Ʈ
            
            for (int s = 0; s < SHAPE_CNT; s++) {
                int count = 0;
                for (int i = 0; i < LINE; i++) {
                    int r = 0;
                    for (; r < RANK_CNT; r++) if (strcmp(field[i].rank, rank_type[r]) == 0) break;
                    if (r >= start && r < start + 5 && strcmp(field[i].shape, shape_type[s]) == 0) {
                        count++;
                    }
                }
                if (count == 5)	{
                	straightFlash = true;
//                	familyFreq[8]++;
//                	if(bestHand < 8)	bestHand = 8; // ��Ʈ����Ʈ �÷���
				} 
            }
            if(straightFlash)	{
            	familyFreq[8]++;
            	if(bestHand < 8)	bestHand = 8; // ��Ʈ����Ʈ �÷���
            	break;
			}
        }
    }

    // ��ī��
    for (int i = 0; i < RANK_CNT; i++){
    	if (rank_count[i] == 4)	{
    		familyFreq[7]++;
    	    if(bestHand < 7)	bestHand = 7;
		}
	} 

    // Ʈ���� + ����� = Ǯ�Ͽ콺
    bool has_triple = false, has_pair = false;
    for (int i = 0; i < RANK_CNT; i++) {
        if (rank_count[i] == 3) has_triple = true;
        if (rank_count[i] == 2) has_pair = true;
    }
    if (has_triple && has_pair)	{
    		familyFreq[6]++;
    	    if(bestHand < 6)	bestHand = 6;
		}

    // �÷���
    for (int i = 0; i < SHAPE_CNT; i++)	{
    	if (shape_count[i] >= 5)	{
    		familyFreq[5]++;
    	    if(bestHand < 5)	bestHand = 5;
		}
	}

    // Ʈ����
    if (has_triple)	{
    	familyFreq[3]++;
    	if(bestHand < 3)	bestHand = 3;
	}

    // ���� ���� ����
    int pair_cnt = 0;
    for (int i = 0; i < RANK_CNT; i++) {
    	if (rank_count[i] == 2) pair_cnt++;
	}

    if (pair_cnt == 3)	{
    	familyFreq[2]++;
    	if(bestHand < 2)	bestHand = 2;
	}
    if (pair_cnt == 2)	{
    	familyFreq[1]++;
    	if(bestHand < 1)	bestHand = 1;
	}
    if (pair_cnt == 1)	{
    	familyFreq[0]++;
    	if(bestHand < 0)	bestHand = 0;
	}
	
	if(bestHand != -1)	topFamilyCount[bestHand]++;
	return;
}

void printSimulate(int trial)	{
	unsigned long sum = 0;
	printf("%d��° �ùķ��̼�\n\n", trial);
	printf("���� ����Ƚ��\n");
	printf("1. �� ���\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[0], topFamilyCount[0]);
	printf("2. �� ���\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[1], topFamilyCount[1]);
	printf("3. ���� ���\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[2], topFamilyCount[2]);
	printf("4. Ʈ����\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[3], topFamilyCount[3]);
	printf("5. ��Ʈ����Ʈ\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[4], topFamilyCount[4]);
	printf("6. �÷���\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[5], topFamilyCount[5]);
	printf("7. Ǯ �Ͽ콺\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[6], topFamilyCount[6]);
	printf("8. �� ī��\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[7], topFamilyCount[7]);
	printf("9. ��Ʈ����Ʈ �÷���\n\t���� Ƚ��: %u, �ֻ��� ����: %u\n\n", familyFreq[8], topFamilyCount[8]);
	for(int i = 0; i < HAND_CNT; i++)	sum += topFamilyCount[i];
	printf("�� ���� ���� Ƚ��: %u\n", sum);
	printf("-------------------------------------------------------------------------\n");
}

void printResult()	{
	printf("\n");
	printf("1. �� ���\n\t��� ���� Ƚ��: %lf ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[0] / SIMUL, (double)totalTopFamilyCount[0] / SIMUL);
	printf("2. �� ���\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[1] / SIMUL, (double)totalTopFamilyCount[1] / SIMUL);
	printf("3. ���� ���\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[2] / SIMUL, (double)totalTopFamilyCount[2] / SIMUL);
	printf("4. Ʈ����\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[3] / SIMUL, (double)totalTopFamilyCount[3] / SIMUL);
	printf("5. ��Ʈ����Ʈ\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[4] / SIMUL, (double)totalTopFamilyCount[4] / SIMUL);
	printf("6. �÷���\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[5] / SIMUL, (double)totalTopFamilyCount[5] / SIMUL);
	printf("7. Ǯ �Ͽ콺\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[6] / SIMUL, (double)totalTopFamilyCount[6] / SIMUL);
	printf("8. �� ī��\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[7] / SIMUL, (double)totalTopFamilyCount[7] / SIMUL);
	printf("9. ��Ʈ����Ʈ �÷���\n\t��� ���� Ƚ��: %lf, ��� �ֻ��� ����: %lf\n\n", (double)totalFamilyFreq[8] / SIMUL, (double)totalTopFamilyCount[8] / SIMUL);
}

void simulate() {
	srand(time(NULL));
    for (int trial = 0; trial < SIMUL; trial++) {
		
        initDeck();
        initHand();
        initNote();

        card field[7];
        for (int i = 0; i < LINE; i++) field[i] = draw_card();

        for (int note = 0; note < 1500; note++) {
            int idx = note_lines[note];

    		field[idx].durable--;
    		if (field[idx].durable == 0) {
        		grave.push_back(field[idx]);
        		field[idx] = draw_card();
    		} // �ش� ���ο� �ִ� ī���� �������� 0�� ���, grave�� �̵� �� ���ο�  ī�� draw 

            // ���� �Ǵ�
            check_hand(field);
        }
        for(int i = 0; i < HAND_CNT; i++)	{
        	totalFamilyFreq[i] += familyFreq[i];
        	totalTopFamilyCount[i] += topFamilyCount[i];
		}
        // printSimulate(trial + 1);
    }
    printResult();
}

int main()	{
	simulate();
	
	return 0;
}
