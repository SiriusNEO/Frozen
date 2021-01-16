#ifndef OBJECTS_H
#define OBJECTS_H

enum gridStatusType {none0, player1, apple2, gapple3, rabbit4, alienfruit5, heart6, gheart7, material8, ice9, seed10, gseed11, crown12,
                crossbomb13, laser14, catastrophe15, tree16, gtree17, wall18, carpet19, pylon20, ptree21, cell22, motor23, door24, border25,
                transparentwall26, transparentcarpet27, chargedcell28, opendoor29, chargedwall30, chargedcarpet31, chargedptree32, chargedmotor33,
                boom34, zzz35, doom36, snowdoom37, myzzzh38, myzzzv39, prioritypylon40, chargedpylon41, chargedprioritypylon42, river43,
                gleaf44, doggy45, winterking46, boss47,specialattack48
                };
enum faceType {up0, left1, down2, right3};
enum gameType {common0, invincible1, starve2, pause3, death4};
const int flagX[] = {0, -1, 0, 1}, flagY[] = {-1, 0, 1, 0};

//Base class
struct Node {
    int x, y, dis;
    friend bool operator > (const Node& obj1, const Node& obj2) {
        return obj1.dis > obj2.dis;
    }
};

struct Object {
    int x, y;
    Object(int _x, int _y):x(_x), y(_y){}
};

struct Creature:public Object {
    int hp;
    gridStatusType inGrid;
    Creature(int _x, int _y, int _hp):Object(_x, _y), hp(_hp), inGrid(none0){}
};

//Real
struct Player:public Creature {
    int hunger, seedCnt, gseedCnt, materialCnt, gappleCnt;
    faceType face;
    Player(int _x, int _y, int _hp, int _hunger):Creature(_x, _y, _hp), hunger(_hunger), face(down2),
    seedCnt(0), gseedCnt(0), materialCnt(0), gappleCnt(0){}
};

//for CrossBomb, Laser and Fear/Snowstorm
struct Bomb:public Object {
    int timeRemain;
    Bomb(int _x, int _y, int _timeRemain):Object(_x, _y), timeRemain(_timeRemain){}
};

//for Tree and GTree
struct Tree:public Object {
    int nextProduce;
    Tree(int _x, int _y, int _initTime):Object(_x, _y), nextProduce(_initTime) {}
};
#endif // OBJECTS_H
