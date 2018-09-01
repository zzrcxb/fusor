// 
// Implementation of DES coded by:
//     - David Wong, moi@davidwong.fr
//     - Jacques Monin, jacques.monin@u-bordeaux.fr
//     - Hugo Bonnin, hugo.bonnin@u-bordeaux.fr
//
// Tables bellow can be verified on wikipedia:
// http://en.wikipedia.org/wiki/DES_supplementary_material#Initial_permutation_.28IP.29
//

#ifndef DES_H
#define DES_H


//
//             USEFUL DEFINES 
//

#define FIRSTBIT 0x8000000000000000 // 1000000000...

#include <stdbool.h>

extern const int DesSbox[8][4][16];
extern const int DesExpansion[48];
extern const int NotDuplicated[16];
extern const int Pbox[32];
extern const int PermutationInitial[64];
extern const int PermutationFinal[64];

//
//             PROTOTYPES
//

// addbit helper
// takes the bit number "position_from" from "from"
// adds it to "block" in position "position_to"
void addbit(uint64_t *block, uint64_t from,
	    int position_from, int position_to);

// Initial and Final Permutations
void Permutation(uint64_t* data, bool initial);

// Verify if the parity bits are okay
bool key_parity_verify(uint64_t key);

// Key Schedule ( http://en.wikipedia.org/wiki/File:DES-key-schedule.png )
// input :
//   * encrypt : false if decryption
//   * next_key : uint64_t next_key 0
//   * round : [[0, 15]]
// changes :
//   * [key] is good to be used in the XOR in the rounds
//   * [next_key] is the combined leftkey+rightkey to be used
//     in the key_schedule for next round
void key_schedule(uint64_t* key, uint64_t* next_key, int round);

#endif
