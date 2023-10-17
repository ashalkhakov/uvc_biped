/*
June 1,2021
Delete variable K2Ｗ[2]
Fixed restart ()

ODEによるUVC(上体垂直制御)の検証 Verification of UVC (upper body vertical control) by ODE
UVCの応用、登坂実験 2021 4/29 Application of UVC, hill climbing experiment 2021 4/29
*/

#include < ode/ode.h >
#include < drawstuff/drawstuff.h >
#include < stdio.h >
#include < stdlib.h >
#include < windows.h >
#include < fstream >
#include < math.h >
#include < conio.h >
#include  "biped.h"
#include  "core.h"

// 関数定義 Function definition
static	void command		(int cmd);
static	void nearCallback	(void *data, dGeomID o1, dGeomID o2);
static	void simLoop		(int pause);
static	void setJoint		(jointStr *j, char k, bodyStr *b1, bodyStr *b2, char a, double x, double y, double z);
static	void setBody		(bodyStr  *b, char k, char	 c,	 double l,	 double w, double h, double r, double x, double y, double z,  int ge,   double ma);
static	void createBody		();
void		 destroyBot		();
void		 restart		();
static	void start			();

// select correct drawing functions
#ifdef  dDOUBLE			// 単精度と倍精度の両方に対応するためのおまじない Character for supporting both single precision and double precision
#define dsDrawBox		dsDrawBoxD
#define dsDrawSphere	dsDrawSphereD
#define dsDrawCylinder	dsDrawCylinderD
#define dsDrawCapsule	dsDrawCapsuleD
#endif

// 間接角度 indirect angle
double K0W[2]={0,0};	// 股関節前後方向書込用 For writing in the anteroposterior direction of the hip joint
double K1W[2]={0,0};	// 股関節横方向書込用 For hip joint lateral writing
// double K2W[2]={0,0};	// 股関節横方向書込用 For hip joint lateral writing
double HW [2]={0,0};	// 膝関節書込用 For knee joint writing
double A0W[2]={0,0};	// 足首上下方向書込用 For writing in the upper and lower direction of the ankle
double A1W[2]={0,0};	// 足首横方向書込用 For ankle lateral writing
double U0W[2]={0,0};	// 肩前後方向書込用 For writing in shoulder anteroposterior direction
double U1W[2]={0,0};	// 肩横後方向書込用 For shoulder horizontal and posterior writing
double U2W[2]={0,0};	// 肩ヨー向書込用 For writing in shoulder yaw direction

// センサ関連 Sensor related
double fbRad=0;			// 頭前後角度 head front and back angle
double lrRad=0;			// 頭左右角度 head left and right angle
double fbAV=0;			// 頭前後角速度 head front and rear angular velocity
double lrAV=0;			// 頭左右角速度 Head left and right angular velocity
double asiPress_r=0;	// 右足裏圧力 right foot pressure
double asiPress_l=0;	// 左足裏圧力 left foot pressure

// 各種変数定義 Various variable definitions
double softERP;			// 柔らかさ、沈み込み softness, sinking
double softCFM;			// 柔らかさ、弾力 softness, elasticity
double bounce;			// 反発係数 coefficient of repulsion
double bounce_vel;		// 反発最低速度 minimum bounce speed

static dWorldID world;				// 動力学計算用ワールド World for dynamics calculation
static dSpaceID space;				// 衝突検出用スペース Space for collision detection
static dJointGroupID contactgroup;	// コンタクトグループ Contact group
static dGeomID ground;				// 地面 ground

dMatrix3 R;
const double* Rot;		// 回転行列取得 Get rotation matrix
int		uvcOff=0;		// UVC起動フラグ UVC activation flag
unsigned char walkF=0;	// 歩行フラグ	（b0:歩行  b1:未  b2:未）walk flag (b0:walk b1:not yet b2:not yet)
int		bodyCount;		// ボディ配列カウント値 Body array count value
int		jointCount;		// ジョイント配列カウント値 Joint array count value
static struct dJointFeedback feedback[50];	// ジョイントフィードバック構造体 Joint feedback structure
double	frRatI;			// 上体角補正用積分係数 Integral coefficient for body angle correction
double	frRatA;			// 上体角オフセット値 Upper body angle offset value
double	fhRat;			// 足上げ高さ補正値 Foot lift height correction value
double	fwMax;			// 歩幅最大値 Maximum stride length


//###############  各種構造体 Various structures ###############
bodyStr *body[50];	// bodyStrアドレス格納配列 bodyStr address storage array
bodyStr solep_r;	// 足裏圧力センサ Sole pressure sensor
bodyStr solep_l;	
bodyStr sole_r;		// 足裏 sole
bodyStr sole_l;	
bodyStr A1_r;		// 足首ロール	 ankle roll
bodyStr A1_l;	
bodyStr A0_r;		// 足首ピッチ ankle pitch
bodyStr A0_l;
bodyStr S_r;		// 脛 shin
bodyStr S_l;
bodyStr H_r;		// 膝 knees
bodyStr H_l;
bodyStr M_r;		// 腿 leg
bodyStr M_l;
bodyStr K0_r;		// 股関節ピッチ hip pitch
bodyStr K0_l;
bodyStr K1_r;		// 股関節ロール hip roll
bodyStr K1_l;
bodyStr DOU;		// 胴 torso
bodyStr HEADT;		// 頭 head
bodyStr DAI;		// 台 tower
bodyStr DAI2;		// 台 tower
bodyStr base;		// 遮断機柱 circuit breaker pillar
bodyStr pole;		// 遮断機棒 circuit breaker rod

jointStr *joint[50];// jointStrアドレス格納配列 jointStr address storage array
jointStr soleJ_r;	// 足裏センサ sole sensor
jointStr soleJ_l;
jointStr A1J_r;		// 足首ロール Ankle roll
jointStr A1J_l;
jointStr A0J_r;		// 足首ピッチ Ankle pitch
jointStr A0J_l;
jointStr SJ_r;		// 脛固定 Shin fixation
jointStr SJ_l;
jointStr HJ_r;		// 膝 knee
jointStr HJ_l;
jointStr MJ_r;		// 腿結合 Thigh joint
jointStr MJ_l;
jointStr M2J_r;		// 腿結合2 Thigh joint 2
jointStr M2J_l;
jointStr K0J_r;		// 股関節ピッチ Hip joint pitch
jointStr K0J_l;
jointStr K1J_r;		// 股関節ロール Hip roll
jointStr K1J_l;
jointStr K2J_r;		// 股関節ヨー Hip joint yaw
jointStr K2J_l;
jointStr HEADJ;		// 頭固定 Head fixation
jointStr DAIJ;		// 台の固定 Fixing the stand
jointStr DAI2J;		// 台2の固定 Fixing stand 2
jointStr baseJ;		// 柱の固定 Fixing the column
jointStr poleJ;		// ポールのジョイント pole joint

//###############  クラスの実体化　class instantiation ###############
core co;			// 歩行制御クラスの実体化 Instantiation of gait control class


//--------------------------------- command ----------------------------------------
static void command (int cmd){
	static int mag = 30;

	switch (cmd) {
		//// 外力印加 External input ////
		case 'j':case 'J':
			printf("F<-\n"); // Ｆ←\n
			dBodyAddForce(DOU.b, -20,0,0);
			break;
		case 'k':case 'K':
			printf("F->\n"); // Ｆ→\n
			dBodyAddForce(DOU.b,  20,0,0);
			break;
		case 'p':case 'P':
			printf("F^\n"); // Ｆ↑
			dBodyAddForce(DOU.b, 0,20,0);
			break;
		case 'l':case 'L':
			printf("F.\n"); // Ｆ↓
			dBodyAddForce(DOU.b,  0,-20,0);
			break;

		//// 操作 operate ////
		case 'w':				// 歩行開始 Start walking
			printf("Start walking\n"); // 歩行開始
			walkF=0x01;
			poleJ.t=5;
			break;

		case '1':				//外力実験 External force experiment
			printf("External force experiment\n"); // 外力実験
			restart ();
			fwMax=30;
			frRatA=0;
			frRatI=0;
			fhRat=0;
			setBody  (&base,			'y','d',	260,	 0,	  0,	24,		0,	180,	110,		0,	0.01);	//遮断機棒
			setBody  (&pole,			'y','y',	320,	 0,	  0,	8,		0,	300,	210,		1,	0.0001);	//ボール
			dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
			dBodySetRotation(pole.b, R);
			setJoint(&baseJ,			'g',	&base,	&base,	'x',		0,	180,	0);		//遮断機柱固定用
			setJoint(&poleJ,			'h',	&pole,	&base,	'z',		0,	180,	210);	//遮断機棒ヒンジ
			poleJ.tm=1;
			poleJ.tk=0.25;
			break;

		case '2':				// 前進実験 forward experiment
			printf("Forward experiment\n"); // 前進実験
			restart ();
			fwMax=30;
			frRatA=0.0015;
			frRatI=0;
			fhRat=0;
			goto saka;
			
		case '3':				// 登坂実験 Climbing experiment
			printf("Climbing experiment\n"); // 登坂実験
			restart ();
			fwMax=30;
			frRatA=0.0015;
			frRatI=0.2;
			fhRat=0;
			goto saka;

		case '4':				// 段差実験 step experiment
			printf("Step experiment\n"); // 段差実験
			restart ();
			fwMax=30;
			frRatA=0.0015;
			frRatI=0.2;
			fhRat=7;
			setBody  (&DAI2,		'b','g',500,300,	 20,0,		0,	0,	-10,		 1,	100);	//水平台
			setJoint(&DAI2J,		'g',	&DAI2,&DAI2,	'x',	50,	0,	0);						//台固定用
			break;

		case '5':				// 最終実験 final experiment
			printf("Final experiment\n"); // 最終実験
			restart ();
			fwMax=30;
			frRatI=0.2;
			frRatA=0.0015;
			fhRat=7;
saka:
			setBody  (&DAI,			'b','g',680,300,	 100,   0,	 480,	0,   -70,	1,	100);		//傾斜台
			dRFromAxisAndAngle(R, 0, 1, 0, -0.05);// 回転 rotate
			dBodySetRotation(DAI.b, R);
			setJoint(&DAIJ,			'g',	&DAI,		&DAI,	'x',	  50,	0,	   0);				//台固定用
			break;

		case 'r':case 'R':		// 初期化
			printf("Initialization\n"); // 初期化
			restart ();
			break;
		case 'q':case 'Q':		// 終了
			printf("End\n"); // 終了
			exit(0);
			break;
		case 'u':case 'U':		// UVC ON/OFF
			printf("UVC ON/OFF\n");
			if(uvcOff==0){
				uvcOff=1;
			}
			else{
				uvcOff=0;
			}
			break;
	}
}


//----------------------------------- nearCallback --------------------------------------
static void nearCallback (void *data, dGeomID o1, dGeomID o2){
	int i,n;
	const int N = 10;
	dContact contact[N];
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if (b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact)) return;
	n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
	if (n > 0) {
		for (i=0; i<n; i++) {
			contact[i].surface.mode		= dContactBounce | dContactSoftERP | dContactSoftCFM;
			contact[i].surface.soft_cfm	= 0.00005;												// 柔らかさ、弾力 softness, elasticity
			contact[i].surface.soft_erp	= 0.1;													// 柔らかさ、沈み込み softness, sinking
			if(0 != pole.b &&(b1==pole.b||b2==pole.b))	contact[i].surface.mu		= 0.2;			// ポールの摩擦 pole friction
			else									contact[i].surface.mu		= 5;			// 地面間摩擦 friction between floors
			contact[i].surface.bounce		= 0;												// bouncing the objects
			contact[i].surface.bounce_vel	= 0;												// bouncing velocity
			dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);					// ジョイント生成 Joint generation
			dJointAttach (c,dGeomGetBody(contact[i].geom.g1),dGeomGetBody(contact[i].geom.g2));	// ジョイントを結合する join joints
	    }
	}
}


//--------------------------------- control ----------------------------------------
// hinge control 
static void control(){
	double kp = 100.0;
	double k;
	int i;
	for (i=0;i<jointCount;i++){
		switch (joint[i]->k) {
			case 'h':
				k = kp * (joint[i]->t - dJointGetHingeAngle(joint[i]->j));
				if(abs(k) > joint[i]->tm){
					if(k > 0){k = joint[i]->tm;}
					else{k = -joint[i]->tm;}
				}
				dJointSetHingeParam(joint[i]->j, dParamVel, k );
				dJointSetHingeParam(joint[i]->j, dParamFMax, joint[i]->tk); 
				break;
			case 'd':
				k = joint[i]->t - dJointGetSliderPosition(joint[i]->j);
				dJointSetSliderParam(joint[i]->j, dParamVel,  k * 100);
				dJointSetSliderParam(joint[i]->j, dParamFMax, 300); 
				break;
		}
	}
}


//--------------------------------- simLoop ----------------------------------------
//	simulation loop
static void simLoop (int pause){
	int i;
	char a;
	static int mag = 3;

	double sides[3];
	dJointFeedback *fb;
	dVector3 headVel1;
	dVector3 headVel2;

//	Sleep(1);			// 描画速度の調整 Adjusting drawing speed
	if(_kbhit()){
		a=getchar();	// キー読込 key reading
		command (a);
	}

	if (!pause) {
		//******** この３行は最初に置くべし These three lines should be placed first ********
		dSpaceCollide (space,0,&nearCallback);	// 衝突しそうなジオメトリのペア集団を探す Find pairs of geometries that are likely to collide
		dWorldStep (world,0.01);				// シミュレーションを１ステップ指定時間進める Advance the simulation by one step for the specified time
		dJointGroupEmpty (contactgroup);		// ジョイントグループを空にする Empty the joint group

		//******** 足裏圧力検出 Sole pressure detection ********
		fb = dJointGetFeedback(soleJ_r.j);
		asiPress_r = fb->f1[2];				// 右足(足首Ｚ)圧力 Right foot (ankle Z) pressure
		fb = dJointGetFeedback(soleJ_l.j);
		asiPress_l = fb->f1[2];				// 左足(足首Ｚ)圧力 Left foot (ankle Z) pressure

		//******** 頭前後左右角度検出 Head front, back, left, right, and right angles ********
		Rot = dBodyGetRotation(HEADT.b);		// 回転行列取得 Get rotation matrix
		fbRad = asin(Rot[8]);					// 頭前後角度(後ろに仰向きが正) Anteroposterior angle of the head (positive when facing backwards)
		lrRad = asin(Rot[9]);					// 頭左右角度（右傾きが正） Head left/right angle (right tilt is positive)

		//******** 頭前後左右角速度検出 Head front, rear, left and right angular velocity ********
		Rot = dBodyGetAngularVel(HEADT.b);	// 回転行列取得 Get rotation matrix
		fbAV = Rot[1];						// 頭前後角度(後ろに仰向きが負) Head anteroposterior angle (negative when facing backwards)
		lrAV = Rot[0];						// 頭左右角度（右傾きが正） Head left/right angle (right tilt is positive)

		K0J_r.t	=K0W[0];			// 股関節前後方向書込用 For writing in hip joint anteroposterior direction
		K1J_r.t	=K1W[0];			// 股関節横方向書込用 For hip joint lateral writing
		HJ_r.t	=HW [0];			// 膝関節書込用 For writing knee joint
		A0J_r.t	=A0W[0];			// 足首上下方向書込用 For writing in ankle up and down direction
		A1J_r.t	=A1W[0];			// 足首横方向書込用 For ankle lateral writing

		K0J_l.t	=K0W[1];			// 股関節前後方向書込用 For writing in hip joint anteroposterior direction
		K1J_l.t	=K1W[1];			// 股関節横方向書込用 For hip joint lateral writing
		HJ_l.t	=HW [1];			// 膝関節書込用  For writing knee joint
		A0J_l.t	=A0W[1];			// 足首上下方向書込用 For writing in ankle up and down direction
		A1J_l.t	=A1W[1];			// 足首横方向書込用 For ankle lateral writing

		co.walk();				// 歩行制御 Walk control
		control();				// モータ駆動 motor drive
	}

	for (i=0;i<bodyCount;i++){
		switch (body[i]->c) {
			case 'g':
				dsSetColor (0,1,0);
				break;
			case 'r':
				dsSetColor (1,0,0);
				break;
			case 'b':
				if(uvcOff==0)	dsSetColor(0.3 ,0.3, 2.0);			// 青 green
				else			dsSetColor(2.0, 0.3, 0.3);			// 赤 red
				break;
			case 'y':
	 			dsSetColor (1,1,0);
				break;
			case 'w':
	 			dsSetColor (1,1,1);
				break;
			case 'd':
	 			dsSetColor (0.8,0.8,0.8);
				break;
			default:
				break;
		}
		switch (body[i]->k) {
			case 'b':
				sides[0] = body[i]->l; sides[1] = body[i]->w; sides[2] = body[i]->h;
				dsDrawBox (dBodyGetPosition(body[i]->b),dBodyGetRotation(body[i]->b),sides);						//箱形表示
				break;
			case 's':
	 			dsDrawSphere (dBodyGetPosition(body[i]->b),dBodyGetRotation(body[i]->b),body[i]->r);				//球形表示
				break;
			case 'c':
				dsDrawCapsule (dBodyGetPosition(body[i]->b),dBodyGetRotation(body[i]->b),body[i]->l,body[i]->r);	//カプセル形表示
				break;
			case 'y':
				dsDrawCylinder (dBodyGetPosition(body[i]->b),dBodyGetRotation(body[i]->b),body[i]->l,body[i]->r);	//円柱形表示
				break;
			default:
				break;
		}
	}
}


//----------------------------------- setBody --------------------------------------
//	配列にボディ情報を設定する Set body information in array

static void setBody (bodyStr *b, char k,    char c, double l, double w, double h, double r, double x, double y, double z, int ge, double ma){
// 引数：　                   ボディの種類     色　    長さ　     幅　      高さ     半径　  前後位置   左右位置　上下位置  ジオメト　重量
// Arguments:          Body      type       Color   Length    Width     Height    Radius    Fore/aft  Left/right Up/down  Geometry Weight
	dMass m;

	z += 20;

	// スケール調整 scale adjustment
	l/=1000;
	w/=1000;
	h/=1000;
	r/=1000;
	x/=1000;
	y/=1000;
	z/=1000;

	// 構造体に記録する record in structure
	b-> k = k;			// ボディの種類を記録 Record body type
	b-> c = c;			// ボディの色の種類を記録 Record body color type
	b-> l = l;			// ボディの長さを記録 Record body length
	b-> w = w;			// ボディの幅さを記録 Record the width of the body
	b-> h = h;			// ボディの高さを記録 Record body height
	b-> r = r;			// ボディの半径を記録 record body radius
	b-> ge = ge;			// ジオメトリ設定 有/無 Geometry settings Yes/No
	b-> e = 1;			// ボディ有効設定 Body enable setting

	x += 2;				// 2m手前に置いて地面障害物を避ける Place 2m in front to avoid ground obstacles

	body[bodyCount] = b;	// 構造体のアドレスを格納する Store address of structure
	++bodyCount;			// ボディ数カウントアップ body count up

	// ボディとジオメトリの生成と、質量、位置、関連性を設定 Generate bodies and geometry and set mass, position, and relationships
	switch (b->k) {
		case 'b':	// 箱型 Box-shaped
			b->b = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Generate the existence of an object (Body ID setting)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetBoxTotal (&m,ma,b->l,b->w,b->h);		// 物体の重量設定 Setting the weight of an object
			dBodySetMass (b->b,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->b,x,y,(z));				// 物体の初期位置 initial position of object
			if(ge > 0){
				b->g = dCreateBox (space,b->l,b->w,b->h);	// 物体の幾何情報を生成（ジオメトリID設定） Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->g,b->b);					// 物体の『存在』と『幾何情報』の一致 Matching the “existence” of an object and its “geometric information”
			}
			break;
		case 's':	// 球形 spherical
			b->b = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) create object (set body id)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetSphereTotal (&m,ma,b->r);				// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->b,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->b,x,y,z);					// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->g = dCreateSphere (space,b->r);			// 物体の幾何情報を生成（ジオメトリID設定） generate geometric information
				dGeomSetBody (b->g,b->b);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
			}
			break;
		case 'c':	// カプセル形 capsule shape
			b->b = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Create existence of object (set body ID
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetCapsuleTotal(&m,ma,3,b->r,b->l);		// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->b,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->b,x,y,(b->l/2+z));			// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->g = dCreateCapsule (space,b->r,b->l);	// 物体の幾何情報を生成（ジオメトリID設定） Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->g,b->b);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
			}
			break;
		case 'y':	// 円柱形 cylindrical
			b->b = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Create existence of object (set body ID)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetCylinderTotal(&m,ma,3,b->r,b->l);		// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->b,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->b,x,y,(z));				// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->g = dCreateCylinder (space,b->r,b->l);	// 物体の幾何情報を生成（ジオメトリID設定）Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->g,b->b);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
			}
			break;
		default:
			break;
	}
}


//---------------------------------- setJoint ---------------------------------------
//	ジョイントを生成する generate joint

static void setJoint (jointStr *j, char k, bodyStr *b1, bodyStr *b2, char a, double x, double y, double z){
// 引数：　            対象Joint　Joint種類   Body番号1  　Body番号2　 設定軸  前後位置  左右位置　上下位置
// Argument:      Target Joint  Joint type  Body #1    Body #2  Setting axis Front-back Left-right Up-down
	z+= 20;

	x/=1000;
	y/=1000;
	z/=1000;

	// 構造体に記録する record in structure
	j -> k	= k;			// 種類を記録 record type
	j -> x	= x;			// X座標を記録 record the x coordinate
	j -> y	= y;			// X座標を記録 record the y coordinate
	j -> z	= z;			// X座標を記録 record the z coordinate
	j -> c	= 0;			// 汎用カウンタ General purpose counter
	j -> t	= 0;			// 間接角度 indirect angle
	j -> t2	= 0;			// 間接角度2 indirect angle 2
	j -> mode = 0;			// 駆動モード drive mode
	j -> pn	= 0;			// 足圧力カウンタ foot pressure counter
	j -> tm	= 44.06;		// 8.06最大角速度 -- 8.06 maximum angular velocity
	j -> tm2	= 8.06;		// 8.06最大角速度 -- //8.06 maximum angular velocity
	j -> tk	= 2.45;		// 2.45トルク 25kgfcm   (25/100)/9.8=2.45 -- 2.45 torque 25kgfcm (25/100)/9.8=2.45
	j -> tk2	= 2.45;		// トルク2 torque 2

	x += 2;					// 2m手前に置いて地面障害物を避ける Place 2m in front to avoid ground obstacles
	joint[jointCount] = j;	// 配列のアドレスを格納する Storing the address of the array
	++jointCount;			// 配列カウンタ、カウントアップ array counter, count up

	switch (k) {
		case 'h':	// ヒンジジョイント hinge joint
			j -> j = dJointCreateHinge(world, 0);			// ヒンジジョイントの生成と記録 generate and record hinge joints
			dJointAttach(j -> j, b1->b, b2->b);				// ヒンジジョイントの取付 Installation of hinge joint
			dJointSetHingeAnchor(j -> j, x, y, z);			// 中心点の設定 Setting the center point
			switch (a) {		// 軸を設定 set axis
				case 'x': dJointSetHingeAxis(j -> j, 1, 0, 0); break;	// X軸の設定 set X axis
				case 'z': dJointSetHingeAxis(j -> j, 0, 0, 1); break;	// Z軸の設定 set Z axis
				default : dJointSetHingeAxis(j -> j, 0, 1, 0); break;	// Y軸の設定 set Y axis
			}
			break;
		case 'd':	// スライダージョイント（ダンパー） slider joint (damper)
			j -> j = dJointCreateSlider(world, 0);		// スライダージョイントの生成と記録 Generating and recording slider joints
			dJointAttach(j -> j, b1->b, b2->b);			// スライダージョイントの取付 Installing the slider joint
			dJointSetSliderAxis(j -> j, 0, 0, 1);		// 中心点の設定 Setting the center point
			break;
		case 'f':	// 固定ジョイント fixed joint
			j -> j = dJointCreateFixed(world, 0);		// 固定ジョイントの生成と記録 Generate and record fixed joints
			dJointAttach(j -> j, b1->b, b2->b);			// 固定ジョイントの取付 Installing the fixed joint
			dJointSetFixed(j -> j);						// 固定ジョイントにの設定 Setting to fixed joint
			break;
		case 'g':	// 環境固定ジョイント environment fixed joint
			j -> j = dJointCreateFixed(world, 0);		// 固定ジョイントの生成と記録 Generate and record fixed joints
			dJointAttach(j -> j, b1->b, 0);				// 固定ジョイントの取付（環境固定） Installing a fixed joint (fixed environment)
			dJointSetFixed(j -> j);						// 固定ジョイントにの設定 Setting to fixed joint
			break;
		default:
			break;
	}
}


//---------------------------------- createBody ---------------------------------------
//	各部のパーツサイズ等を指定してロボットを組み立てる
// Assemble the robot by specifying the parts size etc. of each part
static void createBody (){
	double	fw	= 21;		// 脚の間隔（中心からの距離） Leg spacing (distance from center)
	double	fmax= 1000;		// 駆動トルク標準値 Drive torque standard value
	double	kmax= 1000;		// 最大角速度 maximum angular velocity

	softERP		= 0.2;		// 弾力 Elasticity
	softCFM		= 0.0001;	// 沈み込み sinking
	bounce		= 0.01;		// 反発係数 coefficient of restitution
	bounce_vel	= 0.02;		// 反発最低速度 minimum bounce speed
	bodyCount	= 0;			// ボディ配列カウント値 body array count value
	jointCount	= 0;			// ジョイント配列カウント値 joint array count

//	####################
//	#### ボディ生成 body generation ####
//	####################
//						    種類 色		L　 W	H   R		X	Y	Z	ジオメト 重量
//                         Type Color   L   W   H   R       X   Y   Z  Geometry Weight

	setBody  (&HEADT,		'c','w',	15,	0,	0,	21,		0,	0,	340,	0,	0.16);	// 頭 head
	setBody  (&DOU,			'b','b',	40, 84, 130,0,		0,	0,	260,	1,	1.24);	// 胴 torso
	setBody  (&K0_r,		'y','d',	34,	0,	0,	12,		0,	-fw,195,	0,	0.05);	// 股関節ピッチ hip pitch
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(K0_r.b, R);
	setBody  (&K0_l,		'y','d',	34,	0,	0,	12,		0,	fw,	195,	0,	0.05);
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(K0_l.b, R);
	setBody  (&K1_r,		'y','w',	34,	0,	0,	12,		0,	-fw,195,	0,	0.05);	// 股関節ロール hip roll
	dRFromAxisAndAngle(R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(K1_r.b, R);
	setBody  (&K1_l,		'y','w',	34,	0,	0,	12,		0,	fw,	195,	0,	0.05);
	dRFromAxisAndAngle(R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(K1_l.b, R);
	setBody  (&M_r,			'b','d',	20,	26,	90,	0,		0,	-fw,150,	0,	0.08);	// 腿 leg
	setBody  (&M_l,			'b','d',	20,	26,	90, 0,		0,	fw,	150,	0,	0.08);
	setBody  (&H_r,			'y','d',	34,	0,	0,  12,		0,	-fw,105,	0,	0.03);	// 膝 knees
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(H_r.b, R);
	setBody  (&H_l,			'y','d',	34,	0,	0,	12,		0,	fw,	105,	0,	0.03);
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(H_l.b, R);
	setBody  (&S_r,			'b','d',	20,	26, 90,	0,		0,	-fw,60,		1,	0.04);	// 脛 shin
	setBody  (&S_l,			'b','d',	20,	26, 90,	0,		0,	fw,	60,		1,	0.04);
	setBody  (&A0_r,		'y','d',	34,	 0, 0,	12,		0,	-fw,15,		0,	0.02);	// 足首ピッチ ankle pitch
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(A0_r.b, R);
	setBody  (&A0_l,		'y','d',	34,	 0,	0,	12,		0,	fw,	15,		0,	0.02);
	dRFromAxisAndAngle(R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(A0_l.b, R);
	setBody  (&A1_r,		'y','w',	34,	 0,	0,	12,		0,	-fw,15,		0,	0.02);	// 足首ロール ankle roll
	dRFromAxisAndAngle(R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(A1_r.b, R);
	setBody  (&A1_l,		'y','w',	34,	 0,	0,	12,		0,	fw,	15,		0,	0.02);
	dRFromAxisAndAngle(R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(A1_l.b, R);
	setBody  (&sole_r,		'b','w',	55,	 40,2,	0,		0,	-fw,6.0,	0,	0.01);	// 足平 foot
	setBody  (&sole_l,		'b','w',	55,	 40,2,	0,		0,  fw,	6.0,	0,	0.01);
	setBody  (&solep_r,		'b','r',	55,	 40,6,	0,		0,	-fw,3.0,	1,	0.01);	// ソールセンサ sole sensor
	setBody  (&solep_l,		'b','r',	55,	 40,6,	0,		0,  fw,	3.0,	1,	0.01);
	pole.b=0;// 摩擦係数設定条件 Friction coefficient setting conditions

//	######################
//	####ジョイント生成 Joint generation ####
//	######################
//							種類		B番号1		B番号2	軸			X		Y		Z
//                        Type   B number 1 B number 2 Axis         X       Y       Z

	setJoint(&HEADJ,		'f',	&HEADT,		&DOU,	'z',		0,		0,		360);	// 頭固定用 For head fixation
	setJoint(&K0J_r,		'h',	&K1_r,		&K0_r,	'y',		0,		-fw,	195);	// 股関節ピッチ hip joint pitch
	setJoint(&K0J_l,		'h',	&K1_l,		&K0_l,	'y',		0,		fw,		195);
	setJoint(&K1J_r,		'h',	&DOU,		&K1_r,	'x',		0,		-fw+11,	195);	// 股関節ロール hip roll
	setJoint(&K1J_l,		'h',	&K1_l,		&DOU,	'x',		0,		fw-11,	195);
	setJoint(&MJ_r,			'f',	&M_r,		&K0_r,	'y',		0,		-fw,	128);	// 腿固定用 for leg fixation
	setJoint(&MJ_l,			'f',	&M_l,		&K0_l,	'y',		0,		fw,		128);
	setJoint(&M2J_r,		'f',	&H_r,		&M_r,	'y',		0,		-fw,	128);	// 腿固定用 for leg fixation
	setJoint(&M2J_l,		'f',	&H_l,		&M_l,	'y',		0,		fw,		128);
	setJoint(&HJ_r,			'h',	&S_r,		&H_r,	'y',		0,		-fw,	105);	// 膝関節 Knee fixation
	setJoint(&HJ_l,			'h',	&S_l,		&H_l,	'y',		0,		fw,		105);
	setJoint(&SJ_r,			'f',	&S_r,		&A0_r,	'y',		0,		-fw,	60);	// 脛固定用 For shin fixation
	setJoint(&SJ_l,			'f',	&S_l,		&A0_l,	'y',		0,		fw,		60);
	setJoint(&A0J_r,		'h',	&A0_r,		&A1_r,	'y',		0,		-fw,	15);	// 足首ピッチ Ankle pitch
	setJoint(&A0J_l,		'h',	&A0_l,		&A1_l,	'y',		0,		fw,		15);
	setJoint(&A1J_r,		'h',	&A1_r,		&sole_r,'x',		0,		-fw+11,	15);	// 足首ロール Ankle roll
	setJoint(&A1J_l,		'h',	&sole_l,	&A1_l,	'x',		0,		fw-11,	15);
	setJoint(&soleJ_r,		'd',	&solep_r,	&sole_r,'x',		0,		-fw,	6);		// ソール圧力センサ sole pressure sensor
	setJoint(&soleJ_l,		'd',	&solep_l,	&sole_l,'x',		0,		fw,		6);

	dJointSetFeedback(soleJ_r.j,			&feedback[0]);
	dJointSetFeedback(soleJ_l.j,			&feedback[1]);
}


//--------------------------------- destroy ----------------------------------------
//	ロボットの破壊 robot destruction
void destroyBot (){
	int i;
	for (i=0;i<jointCount;i++){
		if(joint[i]->e > 0){dJointDestroy (joint[i]->j);}	// ジョイント破壊 Joint destruction
	}
	for (i=0;i<bodyCount;i++){
		if(body[i]->e > 0){dBodyDestroy (body[i]->b);}		// ボディ有効なら破壊 Destroy if body is valid
		if(body[i]->ge > 0){dGeomDestroy (body[i]->g);}		// ジオメトリ設定されてたら破壊 Destroyed if geometry is set
	}
	dJointGroupDestroy (contactgroup);
}


//--------------------------------- restart ----------------------------------------
//	リスタート restart
void restart (){
	destroyBot ();
	contactgroup = dJointGroupCreate (0);	// 接触点のグループを格納する入れ物 A container that stores a group of contact points
	createBody();						// ロボット生成 robot generation
	dWorldSetGravity (world, 0, 0, -9.8);	// 重力設定 Gravity settings
	co.mode=0;
	co.autoHs=180;
	walkF=0;
	uvcOff=0;
	frRatI=0;
	frRatA=0;
	fhRat=0;
	fwMax=0;
 
	K0W[0]=0;			//股関節前後方向 hip joint anteroposterior direction
	K1W[0]=0;			//股関節横方向 Hip joint lateral direction
	HW [0]=0;			//膝関節 knee joint
	A0W[0]=0;			//足首上下方向 Ankle up and down direction
	A1W[0]=0;			//足首横方向 Ankle lateral direction
	K0W[1]=0;			//股関節前後方向 hip joint anteroposterior direction
	K1W[1]=0;			//股関節横方向 Hip joint lateral direction
	HW [1]=0;			//膝関節 knee joint
	A0W[1]=0;			//足首上下方向 Ankle up and down direction
	A1W[1]=0;			//足首横方向 Ankle lateral direction
}


//--------------------------------- start ----------------------------------------
//	start simulation - set viewpoint
static void start(){
	static float xyz[3] = {2.3, -0.3, 0.18};	// 視点の位置 View position
	static float hpr[3] = {135,0,0};	// 視線の方向 direction of gaze
	dsSetViewpoint (xyz,hpr); // カメラの設定 Camera settings
}


//------------------------------------ main -------------------------------------
int main (int argc, char **argv){
	dMass m;
	dInitODE(); // ODEの初期化 Initializing ODE

	// setup pointers to drawstuff callback functions
	dsFunctions fn;
	fn.version = DS_VERSION;
	fn.start = &start;
	fn.step = &simLoop;
	fn.command = &command; // Windows10から利用できなくなった No longer available from Windows 10
	fn.stop = 0;
	fn.path_to_textures = "c:/ode-0.13/drawstuff/textures";
	if(argc==2)fn.path_to_textures = argv[1];

	//   create world
	world = dWorldCreate();					// シミュレーションワールド生成 Create simulation world
	space = dHashSpaceCreate (0);			// 衝突検出用スペース生成 Create space for collision detection
	contactgroup = dJointGroupCreate (0);	// 接触点のグループを格納する入れ物 Container for storing groups of contact points
	dWorldSetGravity (world, 0, 0, -9.8);	// 重力設定 Gravity settings
	ground = dCreatePlane (space,0,0,1,0);	// 平面のジオメトリ作成 Create plane geometry
	createBody();							// ロボット生成 Create robot

	// run simulation
	dsSimulationLoop (argc,argv,640,640,&fn);

	// 後始末 clean up
	destroyBot ();
	dSpaceDestroy (space);
	dWorldDestroy (world);
	return 0;
}