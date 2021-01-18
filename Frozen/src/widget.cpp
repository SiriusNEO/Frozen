#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent):QWidget(parent), ui(new Ui::Widget) {
    //窗口初始化
    ui->setupUi(this);
    resize(winWidth, winHeight);
    setStyleSheet("QWidget{background:lightgrey}");
    setWindowOpacity(1);//透明度

    //合成表导入
    composedTable[0].push_back(std::make_pair("GApple", 1));
    composedTable[1].push_back(std::make_pair("Seed", 1));
    composedTable[2].push_back(std::make_pair("GSeed", 1));
    composedTable[3].push_back(std::make_pair("Material", 2));
    composedTable[4].push_back(std::make_pair("Material", 5));
    composedTable[5].push_back(std::make_pair("Material", 1));
    composedTable[6].push_back(std::make_pair("Material", 10));
    composedTable[7].push_back(std::make_pair("Seed", 5));
    composedTable[7].push_back(std::make_pair("Material", 20));
    composedTable[8].push_back(std::make_pair("GApple", 1));
    composedTable[8].push_back(std::make_pair("Material", 5));
    composedTable[9].push_back(std::make_pair("Material", 10));
    composedTable[10].push_back(std::make_pair("Seed", 3));
    composedTable[10].push_back(std::make_pair("GSeed", 1));
    composedTable[10].push_back(std::make_pair("Material", 10));

    //状态初始化
    memset(mapStatus, none0, sizeof mapStatus);
    day = 1, nowTime = 0;
    crossBombCnt = laserCnt = catastropheCnt = doggyCnt = 1;
    for (int i = 0; i < edgeBlockNumX*edgeBlockNumY; ++i)
        crossBomb[i] = laserLauncher[i] = catastropheSign[i] = nullptr, treeSet[i] = gtreeSet[i] = nullptr, doggySet[i] = winterKingSet[i] = nullptr;
    player = new Player(playerInitX, playerInitY, 10, 50);
    mapStatus[playerInitX][playerInitY] = player1;
    for (int i = 0; i <= edgeBlockNumX+1; ++i) mapStatus[i][0] = mapStatus[i][edgeBlockNumY+1] = border25;
    for (int i = 0; i <= edgeBlockNumY+1; ++i) mapStatus[0][i] = mapStatus[edgeBlockNumX+1][i] = border25;
    for (int i = 1; i <= edgeBlockNumY; ++i) mapStatus[edgeBlockNumX/2][i] = river43;
    for (int i = 1; i <= edgeBlockNumX/2; ++i) mapStatus[i][edgeBlockNumY/2] = river43;
    for (int i = edgeBlockNumX/2-2; i <= edgeBlockNumX/2+2; ++i)
        for (int j = edgeBlockNumY/2-2;j <= edgeBlockNumY/2+2; ++j)
            mapStatus[i][j] = river43;
    isWinter = false;
    treeGrowTime = 3*appleTick, gtreeGrowTime = 5*appleTick;
    hungerLoss = 3;
    gameStatus = common0;
    /*
    player->hp = player->hunger = player->materialCnt = player->seedCnt = player->gseedCnt = player->gappleCnt = 9999; //test
    gameStatus = invincible1; invTime = 99999;
    */
    gameTimer = new QTimer;
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(update()));
    gameTimer->start(tick);

    dayTimer = new QTimer;
    connect(dayTimer, SIGNAL(timeout()), this, SLOT(dayEvent()));
    dayTimer->start(dayTick);

    /*
    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setMedia(QUrl::fromLocalFile(":/res/Spring.mp3"));
    mediaPlayer->setVolume(30);
    mediaPlayer->play();
    */
}

Widget::~Widget() {
    delete gameTimer;
    delete dayTimer;
    delete player;
    delete ui;
    delete mediaPlayer;
}

void Widget::dayEvent() {
    //Clock
    ++nowTime;
    if (nowTime % dayTime == 0) {
        day++;
        if (day % 10 == 1) {
            if (isWinter) isWinter = false, treeGrowTime /= 2, gtreeGrowTime /= 2;
            else isWinter = true, treeGrowTime *= 2, gtreeGrowTime *= 2;
        }
        if (day <= 10) {
             if (day % 2 == 0 && crossBombCnt <= maxCrossBombCnt) crossBombCnt ++;
             if (day % 1 == 0 && laserCnt <= maxLaserCnt) laserCnt ++;
             if (day % 5 == 0 && catastropheCnt <= maxCatastropheCnt) catastropheCnt ++;
        }
        else {
            if (day % 7 == 0 && crossBombCnt <= maxCrossBombCnt) crossBombCnt ++;
            if (day % 5 == 0 && laserCnt <= maxLaserCnt) laserCnt ++;
            if (day % 13 == 0 && catastropheCnt <= maxCatastropheCnt) catastropheCnt ++;
        }
        if (day % 4 == 0) hungerLoss += 1;
        if (day % 10 == 0 && doggyCnt <= maxDoggyCnt) doggyCnt ++;
    }
    if (invTime > 0) {
        invTime--;
        if (invTime <= 0) {
            gameStatus = gameStatusTmp;
        }
    }
    if (invTime == 0 && gameStatus == invincible1) gameStatus = common0; //fix forever Inv
    if (gameStatus != invincible1 && gameStatus != pause3) {
        if (player->hunger > 0) gameStatus = common0;
        else gameStatus = starve2;
    }
    if (player->hp <= 0) {
        gameStatus = death4;
        repaint();
        gameTimer->stop();
        dayTimer->stop();
    }
    bool catastropheFlag = false;
    for (int i = 0; i < edgeBlockNumX*edgeBlockNumY; ++i) {
        if (crossBomb[i]) {
            crossBomb[i]->timeRemain--;
            if (crossBomb[i]->timeRemain <= 0) {
                mapStatus[crossBomb[i]->x][crossBomb[i]->y] = none0;
                for (int j = crossBomb[i]->x; j <= edgeBlockNumX; ++j) if (destroy(j, crossBomb[i]->y, boom34)) break;
                for (int j = crossBomb[i]->x; j > 0; --j) if (destroy(j, crossBomb[i]->y, boom34)) break;
                for (int j = crossBomb[i]->y; j <= edgeBlockNumY; ++j) if (destroy(crossBomb[i]->x, j, boom34)) break;
                for (int j = crossBomb[i]->y; j > 0; --j) if (destroy(crossBomb[i]->x, j, boom34)) break;
                repaint();
                if (day <= 15) Wait(50);
                else Wait(15);
                for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j)
                    if (mapStatus[i][j] == boom34) mapStatus[i][j] = none0;
                delete crossBomb[i];
                crossBomb[i] = nullptr;
            }
        }
        if (laserLauncher[i]) {
            laserLauncher[i]->timeRemain--;
            if (laserLauncher[i]->timeRemain <= 0) {
                for (int j = edgeBlockNumX; j > 0; --j)
                    if (destroy(j, laserLauncher[i]->y, zzz35)) break;
                repaint();
                if (day <= 15) Wait(70);
                else Wait(20);
                for (int j = edgeBlockNumX; j > 0; --j)
                    if (chargedValidater(mapStatus[j][laserLauncher[i]->y]) && poweroff(j, laserLauncher[i]->y)) break;
                for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j)
                    if (mapStatus[i][j] == zzz35 || mapStatus[i][j] == myzzzh38 || mapStatus[i][j] == myzzzv39) mapStatus[i][j] = none0;
                if (player->inGrid == chargedcarpet31) player->inGrid = transparentcarpet27;
                delete laserLauncher[i];
                laserLauncher[i] = nullptr;
            }
        }
        if (catastropheSign[i]) {
            catastropheSign[i]->timeRemain--;
            if (catastropheSign[i]->timeRemain <= 0) {
                catastropheFlag = true;
                for (int ii = Max(1, catastropheSign[i]->x-2); ii <= Min(edgeBlockNumX, catastropheSign[i]->x+2); ++ii)
                    for (int jj = Max(1, catastropheSign[i]->y-2); jj <= Min(edgeBlockNumY, catastropheSign[i]->y+2); ++jj) {
                        if (isWinter) destroy(ii, jj, snowdoom37);
                        else destroy(ii, jj, doom36);
                    }
                delete catastropheSign[i];
                catastropheSign[i] = nullptr;
            }
        }
        if (treeSet[i]) {
            treeSet[i]->nextProduce--;
            if (treeSet[i]->nextProduce <= 0) {
                if (mapStatus[treeSet[i]->x][treeSet[i]->y-1] == none0) mapStatus[treeSet[i]->x][treeSet[i]->y-1] = apple2;
                if (mapStatus[treeSet[i]->x-1][treeSet[i]->y] == none0) mapStatus[treeSet[i]->x-1][treeSet[i]->y] = apple2;
                if (mapStatus[treeSet[i]->x][treeSet[i]->y+1] == none0) mapStatus[treeSet[i]->x][treeSet[i]->y+1] = apple2;
                if (mapStatus[treeSet[i]->x+1][treeSet[i]->y] == none0) mapStatus[treeSet[i]->x+1][treeSet[i]->y] = apple2;
                treeSet[i]->nextProduce = treeGrowTime;
            }
        }
        if (gtreeSet[i]) {
            gtreeSet[i]->nextProduce--;
            if (gtreeSet[i]->nextProduce <= 0) {
                if (mapStatus[gtreeSet[i]->x][gtreeSet[i]->y-1] == none0) mapStatus[gtreeSet[i]->x][gtreeSet[i]->y-1] = gapple3;
                if (mapStatus[gtreeSet[i]->x-1][gtreeSet[i]->y] == none0) mapStatus[gtreeSet[i]->x-1][gtreeSet[i]->y] = gapple3;
                if (mapStatus[gtreeSet[i]->x][gtreeSet[i]->y+1] == none0) mapStatus[gtreeSet[i]->x][gtreeSet[i]->y+1] = gapple3;
                if (mapStatus[gtreeSet[i]->x+1][gtreeSet[i]->y] == none0) mapStatus[gtreeSet[i]->x+1][gtreeSet[i]->y] = gapple3;
                gtreeSet[i]->nextProduce = gtreeGrowTime;
            }
        }
        if (doggySet[i]) FindRounte(doggySet[i], doggy45);
        if (winterKingSet[i]) {
            if (nowTime % winterKingMoveTick == 0) FindRounte(winterKingSet[i], winterking46);
            if (nowTime % winterKingSpecialAttackTick == 0) {
                Dijkstra();
                SpecialAttack(winterKingSet[i]->x, winterKingSet[i]->y);
                for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j)
                    if (mapStatus[i][j] == specialattack48) mapStatus[i][j] = ice9;
            }
        }
    }
    if (catastropheFlag) {
        if (isWinter) setStyleSheet("QWidget{background:blue}");
        else setStyleSheet("QWidget{background:red}");
        dayTimer->stop();
        repaint();
        Wait(400);
        for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j) {
            if (mapStatus[i][j] == doom36) mapStatus[i][j] = none0;
            else if (mapStatus[i][j] == snowdoom37) mapStatus[i][j] = ice9;
         }
        setStyleSheet("QWidget{background:lightgrey}");
        dayTimer->start(dayTick);
    }
    if (isWinter) {
        if (nowTime % iceTick == 0) randomSetObject(ice9);
        if (nowTime % gseedTick == 0) randomSetObject(gseed11);
        if (nowTime % crownTick == 0) randomSetObject(crown12);
    }
    else {
        if (nowTime % appleTick == 0) randomSetObject(apple2);
        if (nowTime % gappleTick == 0) randomSetObject(gapple3);
        if (nowTime % rabbitTick == 0) randomSetObject(rabbit4);
        if (nowTime % seedTick == 0) randomSetObject(seed10);
    }
    if (nowTime % hungerTick == 0) {
        if (gameStatus == common0) player->hunger -= hungerLoss;
        else if (gameStatus == starve2) player->hp --;
    }
    if (nowTime % heartTick == 0) randomSetObject(heart6);
    if (nowTime % gheartTick == 0) randomSetObject(gheart7);
    if (nowTime % materialTick == 0) randomSetObject(material8);
    if (nowTime % crossBombTick == 0) randomSetObject(crossbomb13);
    if (nowTime % laserTick == 0) randomSetObject(laser14);
    if (nowTime % catastropheTick == 0) randomSetObject(catastrophe15);
    if (nowTime % doggyTick == 0) randomSetObject(doggy45);
    if (nowTime % winterKingTick == 0) randomSetObject(winterking46);
}

void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPen pen;
    QBrush brush;
    QFont font1("Comic Sans MS",14,QFont::ExtraLight,false);
    QFont font2("Comic Sans MS",10,QFont::ExtraLight,false);
    QFont font3("Comic Sans MS",18,QFont::ExtraLight,false);
    QImage landImage, playerImage;
    //Draw UI
    painter.drawImage(barX*blockSize, barY*blockSize, QImage("res/bar.png"));
    //Item
    painter.drawImage(itemImaX*blockSize, itemImaY*blockSize, QImage(("res/"+objectImage[composedList[handCur]]).c_str()));
    painter.setFont(font1);
    //Cost Table
    painter.drawText(costTextX*blockSize, costTextY*blockSize, QString("cost"));
    painter.setFont(font2);
    for (auto i = 0; i < composedTable[handCur].size(); ++i) {
        bool enoughFlag = true;
        if (composedTable[handCur][i].first == "GApple" && player->gappleCnt < composedTable[handCur][i].second) enoughFlag = false;
        if (composedTable[handCur][i].first == "Seed" && player->seedCnt < composedTable[handCur][i].second) enoughFlag = false;
        if (composedTable[handCur][i].first == "GSeed" && player->gseedCnt < composedTable[handCur][i].second) enoughFlag = false;
        if (composedTable[handCur][i].first == "Material" && player->materialCnt < composedTable[handCur][i].second) enoughFlag = false;
        if (enoughFlag) painter.setPen(QColor(Qt::darkGreen));
        else painter.setPen(QColor(Qt::red));
        painter.drawText(firstIllustrationX*blockSize, (firstIlustrationY+i*illustrationGap)*blockSize,
        QString((composedTable[handCur][i].first+"*"+intToStr(composedTable[handCur][i].second)).c_str()));
    }
    painter.setPen(QColor(Qt::black));
    painter.setFont(font1);
    painter.drawText(handTextX*blockSize, handTextY*blockSize, QString(("Now: "+composedName[handCur]).c_str()));
    //Status Bar
    painter.setFont(font3);
    painter.drawImage(heartX*blockSize, heartY*blockSize, QImage("res/heart.png"));
    painter.drawText((heartX+textOffsetX)*blockSize, (heartY+textOffsetY)*blockSize, QString(("x "+intToStr(player->hp)).c_str()));
    painter.drawImage((heartX+horizonGap)*blockSize, heartY*blockSize, QImage("res/hunger.png"));
    painter.drawText((heartX+horizonGap+textOffsetX)*blockSize, (heartY+textOffsetY)*blockSize, QString(("x "+intToStr(player->hunger)).c_str()));
    painter.drawImage(heartX*blockSize, (heartY+verticalGap)*blockSize, QImage("res/seed.png"));
    painter.drawText((heartX+textOffsetX)*blockSize, (heartY+verticalGap+textOffsetY)*blockSize, QString(("x "+intToStr(player->seedCnt)).c_str()));
    painter.drawImage((heartX+horizonGap)*blockSize, (heartY+verticalGap)*blockSize, QImage("res/gseed.png"));
    painter.drawText((heartX+horizonGap+textOffsetX)*blockSize, (heartY+verticalGap+textOffsetY)*blockSize, QString(("x "+intToStr(player->gseedCnt)).c_str()));
    painter.drawImage(heartX*blockSize, (heartY+2*verticalGap)*blockSize, QImage("res/material.png"));
    painter.drawText((heartX+textOffsetX)*blockSize, (heartY+2*verticalGap+textOffsetY)*blockSize, QString(("x "+intToStr(player->materialCnt)).c_str()));
    painter.drawImage((heartX+horizonGap)*blockSize, (heartY+2*verticalGap)*blockSize, QImage("res/gapple.png"));
    painter.drawText((heartX+horizonGap+textOffsetX)*blockSize, (heartY+2*verticalGap+textOffsetY)*blockSize, QString(("x "+intToStr(player->gappleCnt)).c_str()));
    //Draw the clock
    if (isWinter)
        painter.drawImage(heartX*blockSize, (heartY+3*verticalGap)*blockSize, QImage("res/winterclock.png"));
    else
        painter.drawImage(heartX*blockSize, (heartY+3*verticalGap)*blockSize, QImage("res/springclock.png"));
    painter.translate(heartX*blockSize+80, (heartY+3*verticalGap)*blockSize+80);
    painter.rotate(360.0*nowTime/dayTime);
    painter.drawImage(-80, -80, QImage("res/cursor.png"));
    painter.rotate(-360.0*nowTime/dayTime);
    painter.translate(-heartX*blockSize-80, -(heartY+3*verticalGap)*blockSize-80);
    painter.setFont(font1);
    painter.drawText(dayTextX*blockSize, (heartY+3*verticalGap+clockGap)*blockSize, QString(("Day  "+intToStr(day)).c_str()));
    //Draw the Status
    painter.setFont(font2);
    switch (gameStatus) {
        case common0: {
            painter.drawImage(statusX*blockSize, statusY*blockSize, QImage("res/common.png"));
            painter.drawText(statusTextX*blockSize, statusTextY*blockSize, QString("I'm the mightiest!"));
            painter.drawText(statusTextX*blockSize, (statusTextY+illustrationGap)*blockSize, QString("WUHU——"));
            break;
        }
        case invincible1: {
            painter.drawImage(statusX*blockSize, statusY*blockSize, QImage("res/invincible.png"));
            painter.drawText(statusTextX*blockSize, statusTextY*blockSize, QString("Hyper,Muteki!"));
            painter.drawText(statusTextX*blockSize, (statusTextY+illustrationGap)*blockSize, QString(("time remain: "+intToStr(invTime)).c_str()));
            break;
        }
        case starve2: {
            painter.drawImage(statusX*blockSize, statusY*blockSize, QImage("res/starve.png"));
            painter.drawText(statusTextX*blockSize, statusTextY*blockSize, QString("Ouch!Where are"));
            painter.drawText(statusTextX*blockSize, (statusTextY+illustrationGap)*blockSize, QString("those rabbits?"));
            break;
        }
        case pause3: {
            painter.drawImage(statusX*blockSize, statusY*blockSize, QImage("res/pause.png"));
            painter.drawText(statusTextX*blockSize, statusTextY*blockSize, QString("The, World!"));
            painter.drawText(statusTextX*blockSize, (statusTextY+illustrationGap)*blockSize, QString("to ki o to ma re!"));
            break;
        }
        case death4: {
            painter.drawImage(statusX*blockSize, statusY*blockSize, QImage("res/death.png"));
            painter.drawText(statusTextX*blockSize, statusTextY*blockSize, QString("Game Over..."));
            painter.drawText(statusTextX*blockSize, (statusTextY+illustrationGap)*blockSize, QString("ki bo~"));
            break;
        }
    }
    painter.drawImage(targetImageX*blockSize, targetImageY*blockSize,
                      QImage(("res/"+objectImage[mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]]]).c_str()));
    //Draw the map
    //floor
    landImage.load(isWinter?"res/iceland.png":"res/grassland.png");
    for (int i = 1; i <= edgeBlockNumX; ++i) for (int j = 1; j <= edgeBlockNumY; ++j) {
        if (carpetValidater(mapStatus[i][j])) painter.drawImage(i*blockSize, j*blockSize, QImage(("res/"+objectImage[mapStatus[i][j]]).c_str()));
        painter.drawImage(i*blockSize, j*blockSize, landImage);
    }
    //object
    for (int i = 1; i <= edgeBlockNumX; ++i)
        for (int j = 1; j <= edgeBlockNumY; ++j) {
            if (mapStatus[i][j] != none0 && mapStatus[i][j] != player1 && mapStatus[i][j] != doggy45) {
                if (mapStatus[i][j] == catastrophe15) {
                    if (isWinter) painter.drawImage(i*blockSize, j*blockSize, QImage("res/snowstorm.png"));
                    else painter.drawImage(i*blockSize, j*blockSize, QImage("res/fear.png"));
                }
                else painter.drawImage(i*blockSize, j*blockSize, QImage(("res/"+objectImage[mapStatus[i][j]]).c_str()));
            }
        }
    //Draw the laser
    for (int i = 1; i <= edgeBlockNumY; ++i)
        painter.drawImage((edgeBlockNumX+1)*blockSize, i*blockSize, QImage("res/safe.png"));
    for (int i = 0; i < edgeBlockNumX*edgeBlockNumY; ++i) {
        if (laserLauncher[i]) painter.drawImage((edgeBlockNumX+1)*blockSize, laserLauncher[i]->y*blockSize, QImage("res/caution.png"));
    }
    for (int i = 1; i <= edgeBlockNumX; ++i) {
        for (int j = 1; j <= edgeBlockNumY; ++j) if (mapStatus[j][i] == zzz35) {
            painter.drawImage((edgeBlockNumX+1)*blockSize, i*blockSize, QImage("res/shooting.png"));
            break;
        }
    }
    //Draw the player
    if (mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]] != border25)
        painter.drawImage((player->x+flagX[player->face])*blockSize, (player->y+flagY[player->face])*blockSize, QImage("res/target.png"));
    if (player->inGrid != none0)
        painter.drawImage(player->x*blockSize, player->y*blockSize, QImage(("res/"+objectImage[player->inGrid]).c_str()));
    if (gameStatus == invincible1) {
        switch (player->face) {
            case up0:playerImage.load("res/player_u_inv.png");break;
            case left1:playerImage.load("res/player_l_inv.png");break;
            case down2:playerImage.load("res/player_d_inv.png");break;
            case right3:playerImage.load("res/player_r_inv.png");break;
        }
    }
    else {
        switch (player->face) {
            case up0:playerImage.load("res/player_u.png");break;
            case left1:playerImage.load("res/player_l.png");break;
            case down2:playerImage.load("res/player_d.png");break;
            case right3:playerImage.load("res/player_r.png");break;
        }
    }
    painter.drawImage(player->x*blockSize, player->y*blockSize, playerImage);
    //Draw the doggy
    for (int i = 0; i < edgeBlockNumX*edgeBlockNumY; ++i)
        if (doggySet[i]) {
            if (isWinter) painter.drawImage(doggySet[i]->x*blockSize, doggySet[i]->y*blockSize, QImage("res/winterdoggy.png"));
            else painter.drawImage(doggySet[i]->x*blockSize, doggySet[i]->y*blockSize, QImage("res/springdoggy.png"));
        }
    QWidget::paintEvent(event);
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if (gameStatus == pause3 && event->key() != Qt::Key_Space) return;
    if ((event->key() == Qt::Key_Up || event->key() == Qt::Key_Left || event->key() == Qt::Key_Down || event->key() == Qt::Key_Right)) {
        faceType dir;
        switch (event->key()) {
            case Qt::Key_Up: dir = up0;break;
            case Qt::Key_Left: dir = left1;break;
            case Qt::Key_Down: dir = down2;break;
            case Qt::Key_Right: dir = right3;break;
            default:break;
        }
        player->face = dir;
        if (passValidater(mapStatus[player->x+flagX[dir]][player->y+flagY[dir]])) {
            mapStatus[player->x][player->y] = player->inGrid;
            player->x += flagX[dir], player->y += flagY[dir];
            if (carpetValidater(mapStatus[player->x][player->y]))
                player->inGrid = mapStatus[player->x][player->y];
            else
                player->inGrid = none0;
            switch (mapStatus[player->x][player->y]) {
                case apple2:player->hunger += 10;break;
                case gapple3:player->gappleCnt++;break;
                case rabbit4:player->hp++,player->hunger+=50;break;
                case alienfruit5:{
                    player->hp += 8;
                    if (gameStatus != invincible1) player->hunger-=100;
                    break;
                }
                case heart6:player->hp += 3;break;
                case gheart7:player->hp*=2;break;
                case material8:player->materialCnt+=10;break;
                case ice9:{
                    if (gameStatus != invincible1) player->hp--,player->hunger-=20;
                    break;
                }
                case seed10:player->seedCnt++;break;
                case gseed11:player->gseedCnt++;break;
                case crown12:{
                    gameStatusTmp = gameStatus;
                    gameStatus = invincible1;
                    invTime = 20;
                    break;
                }
            }
            mapStatus[player->x][player->y] = player1;
        }
        return ;
    }
    switch (event->key()) {
        case Qt::Key_Space: {
            if (gameStatus != pause3) {
                gameStatusTmp = gameStatus;
                dayTimer->stop();
                gameStatus = pause3;
            }
            else {
                gameStatus = gameStatusTmp;
                dayTimer->start(dayTick);
            }
            break;
        }
        case Qt::Key_A: {
            handCur = (handCur - 1 + composedNum) % composedNum;
            break;
        }
        case Qt::Key_S: {
            handCur = (handCur + 1) % composedNum;
            break;
        }
        case Qt::Key_Z: {
            if (composedList[handCur] == gleaf44) {
                if  (mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]] != river43) break;
                bool enoughFlag = true;
                for (auto i : composedTable[handCur]) {
                    if (i.first == "GApple" && player->gappleCnt < i.second) enoughFlag = false;
                    if (i.first == "Seed" && player->seedCnt < i.second) enoughFlag = false;
                    if (i.first == "GSeed" && player->gseedCnt < i.second) enoughFlag = false;
                    if (i.first == "Material" && player->materialCnt < i.second) enoughFlag = false;
                }
                if (enoughFlag) {
                    for (auto i : composedTable[handCur]) {
                        if (i.first == "GApple") player->gappleCnt -= i.second;
                        if (i.first == "Seed") player->seedCnt -= i.second;
                        if (i.first == "GSeed") player->gseedCnt -= i.second;
                        if (i.first == "Material") player->materialCnt -= i.second;
                    }
                    setObject(player->x+flagX[player->face], player->y+flagY[player->face], composedList[handCur]);
                }
                break;
            }
            else if (passValidater(mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]]) &&
                    !carpetValidater(mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]]) &&
                    mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]] != catastrophe15) {
                bool enoughFlag = true;
                for (auto i : composedTable[handCur]) {
                    if (i.first == "GApple" && player->gappleCnt < i.second) enoughFlag = false;
                    if (i.first == "Seed" && player->seedCnt < i.second) enoughFlag = false;
                    if (i.first == "GSeed" && player->gseedCnt < i.second) enoughFlag = false;
                    if (i.first == "Material" && player->materialCnt < i.second) enoughFlag = false;
                }
                if (enoughFlag) {
                    for (auto i : composedTable[handCur]) {
                        if (i.first == "GApple") player->gappleCnt -= i.second;
                        if (i.first == "Seed") player->seedCnt -= i.second;
                        if (i.first == "GSeed") player->gseedCnt -= i.second;
                        if (i.first == "Material") player->materialCnt -= i.second;
                    }
                    if (composedList[handCur] == gapple3) player->hunger += 100;
                    else setObject(player->x+flagX[player->face], player->y+flagY[player->face], composedList[handCur]);
                }
            }
            else interact(player->x+flagX[player->face], player->y+flagY[player->face], player->face);
            break;
        }
        case Qt::Key_X: {
            if (mannualValidater(mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]]))
                removeObject(player->x+flagX[player->face], player->y+flagY[player->face], mapStatus[player->x+flagX[player->face]][player->y+flagY[player->face]]);
            break;
        }
        case Qt::Key_I:player->face = up0;break;
        case Qt::Key_J:player->face = left1;break;
        case Qt::Key_K:player->face = down2;break;
        case Qt::Key_L:player->face = right3;break;
    }
}

