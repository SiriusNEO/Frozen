#ifndef WIDGET_H
#define WIDGET_H

#include "Objects.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <queue>

#include <QWidget>
#include <QKeyEvent>
#include <QRectF>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //Tool Function
    inline int Max(int val1, int val2) {return (val1 > val2)?val1:val2;}
    inline int Min(int val1, int val2) {return (val1 < val2)?val1:val2;}
    inline void Wait(int _time) {time_t tmpTime = clock();while (clock() <= tmpTime + _time);}
    inline std::pair<int, int> randPosition() {
        if (day >= 15) {
            int retX = rand()%edgeBlockNumX+1, retY = rand()%edgeBlockNumY+1;
            int forceEnd = 0;
            while (mapStatus[retX][retY] != none0) {
                ++forceEnd;
                if (forceEnd >= 100000) throw std::invalid_argument("No Free Blocks!");
                retX = rand()%edgeBlockNumX+1;
                retY = rand()%edgeBlockNumY+1;
            }
            return std::make_pair(retX, retY);
        }
        else {
            int retX = rand()%edgeBlockNumX/2+edgeBlockNumX/2, retY = rand()%edgeBlockNumY/2+1;
            int forceEnd = 0;
            while (mapStatus[retX][retY] != none0) {
                ++forceEnd;
                if (forceEnd >= 100000) throw std::invalid_argument("No Free Blocks!");
                retX = rand()%edgeBlockNumX/2+edgeBlockNumX/2;
                retY = rand()%edgeBlockNumY/2+1;
            }
            return std::make_pair(retX, retY);
        }
    }
    inline std::string intToStr(int val) {std::stringstream ss;ss << val;return ss.str();}
    inline int strToInt(const std::string str) {
        int ret = 0;
        for (auto ch : str) ret = ret * 10 + ch - '0';
        return ret;
    }
    void Dijkstra() {
        memset(disData, 127, sizeof disData);
        memset(visData, 0, sizeof visData);
        disData[player->x][player->y] = 0;
        Q.push((Node){player->x, player->y, 0});
        while (!Q.empty()) {
            Node u = Q.top();Q.pop();
            if (visData[u.x][u.y]) continue;
            visData[u.x][u.y] = true;
            for (int i = 0; i < 4; ++i)
                if ((passValidater(mapStatus[u.x+flagX[i]][u.y+flagY[i]]) || creatureValidater(mapStatus[u.x+flagX[i]][u.y+flagY[i]]))
                     && !visData[u.x+flagX[i]][u.y+flagY[i]] && disData[u.x+flagX[i]][u.y+flagY[i]] > u.dis + 1) {
                    disData[u.x+flagX[i]][u.y+flagY[i]] = u.dis + 1;
                    Q.push((Node){u.x+flagX[i], u.y+flagY[i], u.dis+1});
                }
        }
    }
    void FindRounte(Creature* obj, gridStatusType typ) {
        Dijkstra();
        int minDir = 4;
        for (int i = 0; i < 4; ++i)
            if (disData[obj->x][obj->y] == disData[obj->x+flagX[i]][obj->y+flagY[i]] + 1)
                minDir = i;
        if (minDir < 4) {
            if (obj->x + flagX[minDir] == player->x && obj->y + flagY[minDir] == player->y) {
                if (gameStatus != invincible1) {
                    if (typ == doggy45) player->hp -= 5;
                    else if (typ == winterking46) player->hp -= 20;
                }
                return;
            }
            if (!passValidater(mapStatus[obj->x+flagX[minDir]][obj->y+flagY[minDir]])) return;
            if (typ == doggy45 && isWinter && obj->inGrid == none0) mapStatus[obj->x][obj->y] = ice9;
            else mapStatus[obj->x][obj->y] = obj->inGrid;
            obj->x += flagX[minDir], obj->y += flagY[minDir];
            if (typ == winterking46) {
                for (int i = obj->x-1; i <= obj->x+1; ++i)
                    for (int j = obj->y-1; j <= obj->y+1; ++j)
                        if (mapStatus[i][j] == none0) mapStatus[i][j] = ice9;
            }
            if (carpetValidater(mapStatus[obj->x][obj->y]))
                obj->inGrid = mapStatus[obj->x][obj->y];
            else
                obj->inGrid = none0;
            switch (mapStatus[obj->x][obj->y]) {
                case heart6:obj->hp += 3;break;
                case alienfruit5:obj->hp += 8;break;
                case gheart7:obj->hp *= 2;break;
                default:break;
            }
            mapStatus[obj->x][obj->y] = typ;
        }
        else {
            bool beTrapped = true;
            for (int i = 0; i < 4; ++i)
                if (passValidater(mapStatus[obj->x+flagX[i]][obj->y+flagY[i]])) beTrapped = false;
            if (!beTrapped) {
                minDir = rand()%4;
                while (!passValidater(mapStatus[obj->x+flagX[minDir]][obj->y+flagY[minDir]])) minDir = rand()%4;
                if (typ == doggy45 && isWinter && obj->inGrid == none0) mapStatus[obj->x][obj->y] = ice9;
                else mapStatus[obj->x][obj->y] = obj->inGrid;
                obj->x += flagX[minDir], obj->y += flagY[minDir];
                if (carpetValidater(mapStatus[obj->x][obj->y]))
                    obj->inGrid = mapStatus[obj->x][obj->y];
                else
                    obj->inGrid = none0;
                switch (mapStatus[obj->x][obj->y]) {
                    case heart6:obj->hp += 3;break;
                    case alienfruit5:obj->hp += 8;break;
                    case gheart7:obj->hp *= 2;break;
                    default:break;
                }
                mapStatus[obj->x][obj->y] = typ;
            }
        }
    }
    void SpecialAttack(int _x, int  _y) {
        for (int i = 0; i < 4; ++i) {
            if (disData[_x][_y] == disData[_x+flagX[i]][_y+flagY[i]] + 1) {
                if (!destroy(_x+flagX[i], _y+flagY[i], specialattack48)) {
                    repaint();
                    Wait(80);
                    SpecialAttack(_x+flagX[i], _y+flagY[i]);
                    break;
                }
            }
        }
    }
    inline void setObject(int _x,int _y, gridStatusType obj) {
        mapStatus[_x][_y] = obj;
        if (obj == tree16) {
            for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                if (treeSet[j] == nullptr) {
                    treeSet[j] = new Tree(_x, _y, treeGrowTime);
                    break;
                }
        }
        else if (obj == gtree17) {
            for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                if (gtreeSet[j] == nullptr) {
                    gtreeSet[j] = new Tree(_x, _y, gtreeGrowTime);
                    break;
                }
        }
    }
    inline void removeObject(int _x,int _y, gridStatusType obj) {
        if (mapStatus[_x][_y] == gleaf44) mapStatus[_x][_y] = river43;
        else mapStatus[_x][_y] = none0;
        if (obj == tree16) {
            for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                if (treeSet[j] && treeSet[j]->x == _x && treeSet[j]->y == _y) {
                    delete treeSet[j];
                    treeSet[j] = nullptr;
                    break;
                }
        }
        else if (obj == gtree17) {
            for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                if (gtreeSet[j] && gtreeSet[j]->x == _x && gtreeSet[j]->y == _y) {
                    delete gtreeSet[j];
                    gtreeSet[j] = nullptr;
                    break;
                }
        }
    }
    inline void randomSetObject(gridStatusType obj) {
        if (obj == crossbomb13) {
            for (int i = 1; i <= crossBombCnt; ++i) {
                auto randomPos = randPosition();
                int randomTime = rand()%3+3;
                mapStatus[randomPos.first][randomPos.second] = crossbomb13;
                for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                    if (crossBomb[j] == nullptr) {
                        crossBomb[j] = new Bomb(randomPos.first, randomPos.second, randomTime);
                        break;
                    }
            }
            return;
        }
        else if (obj == laser14) {
            bool vis[edgeBlockNumY+1];
            memset(vis, 0, sizeof vis);
            for (int i = 1; i <= laserCnt; ++i) {
                int randomY = rand()%edgeBlockNumY + 1;
                while (vis[randomY]) randomY = rand()%edgeBlockNumY + 1;
                vis[randomY] = true;
            }
            for (int i = 1; i <= edgeBlockNumY; ++i)
                if (vis[i]) {
                    int randomTime = rand()%3+3;
                    for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                        if (laserLauncher[j] == nullptr) {
                            laserLauncher[j] = new Bomb(0, i, randomTime);
                            break;
                        }
                }
            return;
        }
        else if (obj == catastrophe15) {
            for (int i = 1; i <= catastropheCnt; ++i) {
                auto randomPos = randPosition();
                mapStatus[randomPos.first][randomPos.second] = catastrophe15;
                for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                    if (catastropheSign[j] == nullptr) {
                        catastropheSign[j] = new Bomb(randomPos.first, randomPos.second, 5);
                        break;
                    }
            }
            return;
        }
        else if (obj == doggy45) {
            for (int i = 1; i <= doggyCnt; ++i) {
                auto randomPos = randPosition();
                mapStatus[randomPos.first][randomPos.second] = doggy45;
                for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                    if (doggySet[j] == nullptr) {
                        doggySet[j] = new Creature(randomPos.first, randomPos.second, 5);
                        break;
                    }
            }
            return;
        }
        else if (obj == winterking46) {
            auto randomPos = randPosition();
            mapStatus[randomPos.first][randomPos.second] = boss47;
            setStyleSheet("QWidget{background:yellow}");
            dayTimer->stop();
            repaint();
            Wait(2000);
            dayTimer->start(dayTick);
            setStyleSheet("QWidget{background:lightgrey}");
            mapStatus[randomPos.first][randomPos.second] = winterking46;
            for (int j = 0; j < edgeBlockNumX*edgeBlockNumY; ++j)
                if (winterKingSet[j] == nullptr) {
                    winterKingSet[j] = new Creature(randomPos.first, randomPos.second, 30);
                    break;
                }
            return;
        }
        auto randomPos = randPosition();
        mapStatus[randomPos.first][randomPos.second] = obj;
    }
    inline bool passValidater(gridStatusType typ) {
        return !(typ == player1 || typ == crossbomb13 || typ == tree16 || typ == gtree17 || typ == wall18 || typ == pylon20 ||
            typ == ptree21 || typ == cell22 || typ == motor23 || typ == door24 || typ == border25 || typ == transparentwall26 ||
            typ == chargedcell28 || typ == chargedwall30 || typ == chargedptree32 || typ == chargedmotor33 || typ == prioritypylon40 ||
            typ == chargedpylon41 || typ == chargedprioritypylon42 || typ == river43 || typ == doggy45 || typ == winterking46
                );
    }
    inline bool powerValidater(gridStatusType typ) {
        return (typ == pylon20 || typ == prioritypylon40 || typ == cell22 ||
                typ == motor23 || typ == ptree21 || typ == chargedcell28 || typ == chargedmotor33 ||
                typ == chargedptree32 || typ == chargedpylon41 || typ == chargedprioritypylon42
                );
    }
    inline bool mannualValidater(gridStatusType typ) {
        return (typ >= tree16 && typ != border25 && typ != boom34 && typ != zzz35 && typ != doom36 && typ != snowdoom37 &&
                typ != myzzzh38 && typ != myzzzv39);
    }
    inline bool carpetValidater(gridStatusType typ) {
        return (typ == carpet19 || typ == transparentcarpet27 || typ == chargedcarpet31 || typ == opendoor29 || typ == gleaf44 ||
                typ == catastrophe15 || typ == boss47
                );
    }
    inline bool creatureValidater(gridStatusType typ) {
        return (typ == player1 || typ == doggy45 || typ == winterking46);
    }
    inline void interact(int _x, int _y, faceType dir) {
        if (mapStatus[_x][_y] == wall18) mapStatus[_x][_y] = transparentwall26;
        else if (mapStatus[_x][_y] == transparentwall26) mapStatus[_x][_y] = wall18;
        else if (mapStatus[_x][_y] == carpet19) mapStatus[_x][_y] = transparentcarpet27;
        else if (mapStatus[_x][_y] == transparentcarpet27) mapStatus[_x][_y] = carpet19;
        else if (mapStatus[_x][_y] == pylon20) mapStatus[_x][_y] = prioritypylon40;
        else if (mapStatus[_x][_y] == prioritypylon40) mapStatus[_x][_y] = pylon20;
        else if (mapStatus[_x][_y] == door24) mapStatus[_x][_y] = opendoor29;
        else if (mapStatus[_x][_y] == opendoor29) mapStatus[_x][_y] = door24;
        else if (mapStatus[_x][_y] == chargedcell28) {
            switch (dir) {
                case up0: {
                    for (int j = _y-1; j > 0; --j) if (destroy(_x, j, myzzzv39)) break;
                    break;
                }
                case left1: {
                    for (int j = _x-1; j > 0; --j) if (destroy(j, _y, myzzzh38)) break;
                    break;
                }
                case down2: {
                    for (int j = _y+1; j <= edgeBlockNumY; ++j) if (destroy(_x, j, myzzzv39)) break;
                    break;
                }
                case right3: {
                    for (int j = _x+1; j <= edgeBlockNumX; ++j) if (destroy(j, _y, myzzzh38)) break;
                    break;
                }
            }
            repaint();
            Wait(70);
            for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j) {
                if (mapStatus[i][j] == zzz35 || mapStatus[i][j] == myzzzh38 || mapStatus[i][j] == myzzzv39)
                    mapStatus[i][j] = none0;
                else if (mapStatus[i][j] == chargedwall30) mapStatus[i][j] = transparentwall26;
                else if (mapStatus[i][j] == chargedcarpet31) mapStatus[i][j] = transparentcarpet27;
                else if (mapStatus[i][j] == chargedpylon41) mapStatus[i][j] = pylon20;
                else if (mapStatus[i][j] == chargedprioritypylon42) mapStatus[i][j] = prioritypylon40;
                else if (mapStatus[i][j] == chargedmotor33) mapStatus[i][j] = motor23;
                else if (mapStatus[i][j] == chargedptree32) mapStatus[i][j] = ptree21;
            }
            if (player->inGrid == chargedcarpet31) player->inGrid = transparentcarpet27;
            mapStatus[_x][_y] = cell22;
        }
    }
    inline void trigger(int _x, int _y) {
        if (mapStatus[_x][_y] == pylon20 || mapStatus[_x][_y] == prioritypylon40) {
            if (mapStatus[_x][_y] == pylon20) mapStatus[_x][_y] = chargedpylon41;
            else mapStatus[_x][_y] = chargedprioritypylon42;
            int dirPriority[4] = {0, 0, 0, 0};
            for (int i = _y-1; i > 0; --i) {
                if (powerValidater(mapStatus[_x][i])) {
                    if (mapStatus[_x][i] == prioritypylon40) dirPriority[0] = 2;
                    else dirPriority[0] = 1;
                    break;
                }
                if (!passValidater(mapStatus[_x][i]) && mapStatus[_x][i] != transparentwall26) break;
            }
            for (int i = _x-1; i > 0; --i) {
                if (powerValidater(mapStatus[i][_y])) {
                    if (mapStatus[i][_y] == prioritypylon40) dirPriority[1] = 2;
                    else dirPriority[1] = 1;
                    break;
                }
                if (!passValidater(mapStatus[i][_y]) && mapStatus[i][_y] != transparentwall26) break;
            }
            for (int i = _y+1; i <= edgeBlockNumY; ++i) {
                if (powerValidater(mapStatus[_x][i])) {
                    if (mapStatus[_x][i] == prioritypylon40) dirPriority[2] = 2;
                    else dirPriority[2] = 1;
                    break;
                }
                if (!passValidater(mapStatus[_x][i]) && mapStatus[_x][i] != transparentwall26) break;
            }
            for (int i = _x+1; i <= edgeBlockNumX; ++i) {
                if (powerValidater(mapStatus[i][_y])) {
                    if (mapStatus[i][_y] == prioritypylon40) dirPriority[3] = 2;
                    else dirPriority[3] = 1;
                    break;
                }
                if (!passValidater(mapStatus[i][_y]) && mapStatus[i][_y] != transparentwall26) break;
            }
            for (int k = 2; k >= 1; --k) {
                if (dirPriority[0] == k) for (int i = _y-1; i > 0; --i) if (destroy(_x, i, myzzzv39)) break;
                if (dirPriority[1] == k) for (int i = _x-1; i > 0; --i) if (destroy(i, _y, myzzzh38)) break;
                if (dirPriority[2] == k) for (int i = _y+1; i <= edgeBlockNumY; ++i) if (destroy(_x, i, myzzzv39)) break;
                if (dirPriority[3] == k) for (int i = _x+1; i <= edgeBlockNumX; ++i) if (destroy(i, _y, myzzzh38)) break;
            }
            return;
        }
        else if (mapStatus[_x][_y] == cell22) {
            mapStatus[_x][_y] = chargedcell28;
            return;
        }
        else if (mapStatus[_x][_y] == motor23) {
            mapStatus[_x][_y] = chargedmotor33;
            interact(_x, _y-1, up0);
            interact(_x-1, _y, left1);
            interact(_x, _y+1, down2);
            interact(_x+1, _y, right3);
            return;
        }
        else if (mapStatus[_x][_y] == ptree21) {
            mapStatus[_x][_y] = chargedptree32;
            if (mapStatus[_x][_y-1] == none0) mapStatus[_x][_y-1] = apple2;
            if (mapStatus[_x-1][_y] == none0) mapStatus[_x-1][_y] = apple2;
            if (mapStatus[_x][_y+1] == none0) mapStatus[_x][_y+1] = apple2;
            if (mapStatus[_x+1][_y] == none0) mapStatus[_x+1][_y] = apple2;
            return;
        }
        return;
    }
    //True: stop now
    inline bool destroy(int _x, int _y, gridStatusType typ) {
        if (mapStatus[_x][_y] == player1) {
            if (player->inGrid == transparentcarpet27)
                player->inGrid = chargedcarpet31;
            else {
                if (gameStatus != invincible1) {
                    if (typ == boom34) player->hp -= 3;
                    else if (typ == zzz35 || typ == myzzzh38 || typ == myzzzv39) player->hp -= 5;
                    else if (typ == boom34 || typ == snowdoom37) player->hp -= 10;
                    else if (typ == specialattack48) player->hp -= 3;
                }
            }
            return true;
        }
        else if (mapStatus[_x][_y] == doggy45) {
            for (int i = 0; i < edgeBlockNumX*edgeBlockNumY; ++i)
                if (doggySet[i] && doggySet[i]->x == _x && doggySet[i]->y == _y) {
                    if (doggySet[i]->inGrid == transparentcarpet27)
                        doggySet[i]->inGrid = chargedcarpet31;
                    else {
                        if (typ == boom34) doggySet[i]->hp -= 3;
                        else if (typ == zzz35 || typ == myzzzh38 || typ == myzzzv39) doggySet[i]->hp -= 5;
                        else if (typ == boom34 || typ == snowdoom37) doggySet[i]->hp -= 10;
                        if (doggySet[i]->hp <= 0) {
                            mapStatus[doggySet[i]->x][doggySet[i]->y] = material8;
                            delete doggySet[i];
                            doggySet[i] = nullptr;
                        }
                    }
                    return true;
                }
        }
        else if (mapStatus[_x][_y] == winterking46) {
            for (int i = 0; i <edgeBlockNumX*edgeBlockNumY; ++i)
                if (winterKingSet[i] && winterKingSet[i]->x == _x && winterKingSet[i]->y == _y) {
                    if (winterKingSet[i]->inGrid == transparentcarpet27)
                        winterKingSet[i]->inGrid = chargedcarpet31;
                    else {
                        if (typ == boom34) winterKingSet[i]->hp -= 3;
                        else if (typ == zzz35 || typ == myzzzh38 || typ == myzzzv39) winterKingSet[i]->hp -= 5;
                        else if (typ == boom34 || typ == snowdoom37) winterKingSet[i]->hp -= 10;
                        if (winterKingSet[i]->hp <= 0) {
                            mapStatus[winterKingSet[i]->x][winterKingSet[i]->y] = gheart7;
                            delete winterKingSet[i];
                            winterKingSet[i] = nullptr;
                        }
                    }
                    return true;
                }
        }
        else if ((typ != doom36 && typ != snowdoom37) && (mapStatus[_x][_y] == catastrophe15 || mapStatus[_x][_y] == boss47)) return false;
        else if (mapStatus[_x][_y] == opendoor29 || mapStatus[_x][_y] == gleaf44) return true;
        if (typ == zzz35 || typ == myzzzh38 || typ == myzzzv39) {
            if (mapStatus[_x][_y] == transparentwall26) {
                mapStatus[_x][_y] = chargedwall30;
                return false;
            }
            else if (mapStatus[_x][_y] == transparentcarpet27) {
                mapStatus[_x][_y] = chargedcarpet31;
                return false;
            }
            else if (mapStatus[_x][_y] == chargedcarpet31) return false;
            else if (powerValidater(mapStatus[_x][_y])) {
                trigger(_x, _y);
                return true;
            }
            else if (mapStatus[_x][_y] == gapple3) {
                mapStatus[_x][_y] = alienfruit5;
                return true;
            }
            else if (mapStatus[_x][_y] == alienfruit5) return true;
        }
        if (mapStatus[_x][_y] == tree16) {
            mapStatus[_x][_y] = typ;
            for (int k = 0; k < edgeBlockNumX*edgeBlockNumY; ++k)
                if (treeSet[k] && treeSet[k]->x == _x && treeSet[k]->y == _y) {
                    delete treeSet[k];
                    treeSet[k] = nullptr;
                    break;
                }
            return true;
        }
        else if (mapStatus[_x][_y] == gtree17) {
            mapStatus[_x][_y] = typ;
            for (int k = 0; k < edgeBlockNumX*edgeBlockNumY; ++k)
                if (gtreeSet[k] && gtreeSet[k]->x == _x && gtreeSet[k]->y == _y) {
                    delete gtreeSet[k];
                    gtreeSet[k] = nullptr;
                    break;
                }
            return true;
        }
        else if (mapStatus[_x][_y] == carpet19 || mapStatus[_x][_y] == transparentcarpet27 || mapStatus[_x][_y] == chargedcarpet31) {
            mapStatus[_x][_y] = none0;
            return true;
        }
        if (!passValidater(mapStatus[_x][_y])) return true;
        mapStatus[_x][_y] = typ;
        return false;
    }

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

protected slots:
    void dayEvent();

private:
    Ui::Widget *ui;
    //MAP:14 blocks * 14 blocks, a block:50*50
    static const int winWidth = 2000, winHeight = 1000, blockSize = 50, edgeBlockNumX = 28, edgeBlockNumY = 18;
    //UI Trial Data
    constexpr static const double barX = edgeBlockNumX+2, barY = 1, itemImaX = edgeBlockNumX+4.65, itemImaY = 2.54, costTextX = edgeBlockNumX+6, costTextY = 2.6,
    firstIllustrationX = edgeBlockNumX+5.9, firstIlustrationY = 3.0, illustrationGap = 0.4, handTextX = edgeBlockNumX+4.6, handTextY = 4.8,
    heartX = edgeBlockNumX+2.7, heartY = 6.4, textOffsetX = 1.1, textOffsetY = 0.7, horizonGap = 3.8, verticalGap = 1.4, clockGap = 3.6, dayTextX = edgeBlockNumX+3.5,
    statusX = edgeBlockNumX+7, statusY = 11, statusTextX = edgeBlockNumX+6.7, statusTextY = 13.4, targetImageX = edgeBlockNumX+5.5, targetImageY = 16.4;

    //Control
    static const int tick = 50, dayTick = 500, seasonLast = 10, dayTime = 70, appleTick = 7, gappleTick = 7*appleTick, rabbitTick = 4*appleTick,
                     seedTick = 8*appleTick, heartTick = 9*appleTick, materialTick = 6*appleTick, gseedTick = 3*dayTime+7*appleTick, crownTick = 11*appleTick,
                     iceTick = appleTick, crossBombTick = 3*appleTick, laserTick = 5*appleTick, gheartTick = 15*dayTime, catastropheTick = 2*dayTime+6*appleTick,
                     doggyTick = 8*dayTime+1*appleTick, doggyMoveTick = 1, hungerTick = 4, winterKingTick = 35*dayTime, winterKingMoveTick = 5,
                    winterKingSpecialAttackTick = 6*appleTick
    ;
    bool isWinter;
    int treeGrowTime, gtreeGrowTime, crossBombCnt, laserCnt, catastropheCnt, doggyCnt, hungerLoss;
    static const int maxCrossBombCnt = 10, maxLaserCnt = edgeBlockNumY, maxCatastropheCnt = 8, maxDoggyCnt = 10;
    int day, nowTime, invTime;
    QTimer *gameTimer, *dayTimer;
    gameType gameStatus, gameStatusTmp;
    const int playerInitX = 16, playerInitY = 16;

    //Map and Object
    /*GApple Tree GTree Wall Door Carpet Pylon PTree Cell Motor GLeaf*/
    const int composedNum = 11;
    const std::string composedName[11] = {"GApple", "Tree", "GTree", "Wall", "Door", "Carpet", "Pylon", "PTree", "Cell", "Motor", "Gleaf"};
    const gridStatusType composedList[11] = {gapple3, tree16, gtree17, wall18, door24, carpet19, pylon20, ptree21, cell22, motor23, gleaf44};
    int handCur = 0;
    std::vector<std::pair<std::string, int>> composedTable[11];

    const std::string objectImage[233] = {"none", "player.png", "apple.png", "gapple.png", "rabbit.png", "alienfruit.png", "heart.png", "gheart.png",
                                   "material.png", "ice.png", "seed.png", "gseed.png", "crown.png", "crossbomb.png", "laser.png", "catastrophe.png",
                                    "tree.png", "gtree.png", "wall.png", "carpet.png", "pylon.png", "ptree.png", "cell.png", "motor.png", "door.png",
                                    "border.png", "transparentwall.png", "transparentcarpet.png", "chargedcell.png", "opendoor.png", "chargedwall.png",
                                    "chargedcarpet.png", "chargedptree.png", "chargedmotor.png", "boom.png", "zzz.png", "doom.png", "snowdoom.png",
                                    "myzzz_h.png", "myzzz_v.png", "prioritypylon.png", "chargedpylon.png", "chargedprioritypylon.png", "river.png",
                                    "gleaf.png", "doggy.png", "winterking.png", "boss.png", "specialattack.png"
                                   };
    /*
     * enum statusType {none0, player1, apple2, gapple3, rabbit4, alienfruit5, heart6, goldenheart7, material8, ice9, seed10, gseed11, crown12,
                crossbomb13, laser14, catastrophe15, tree16, gtree17, wall18, carpet19, pylon20, ptree21, cell22, motor23, door24, border25,
                transparentwall26, transparentcarpet27, chargedcell28, opendoor29, chargedwall30, chargedcarpet31, chargedptree32, chargedmotor33
                };
    */
    gridStatusType mapStatus[edgeBlockNumX+5][edgeBlockNumY+5];
    Player *player;
    Bomb *crossBomb[edgeBlockNumX*edgeBlockNumY], *laserLauncher[edgeBlockNumX*edgeBlockNumY], *catastropheSign[edgeBlockNumX*edgeBlockNumY];
    Tree *treeSet[edgeBlockNumX*edgeBlockNumY], *gtreeSet[edgeBlockNumX*edgeBlockNumY];
    Creature *doggySet[edgeBlockNumX*edgeBlockNumY], *winterKingSet[edgeBlockNumX*edgeBlockNumY];

    //Dij
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> Q;
    int disData[edgeBlockNumX+5][edgeBlockNumY+5];
    bool visData[edgeBlockNumX+5][edgeBlockNumY+5];
};
#endif // WIDGET_H
