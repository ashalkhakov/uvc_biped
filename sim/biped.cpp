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
double state_K0W[2]={0,0};	// 股関節前後方向書込用 For writing in the anteroposterior direction of the hip joint
double state_K1W[2]={0,0};	// 股関節横方向書込用 For hip joint lateral writing
// double state_K2W[2]={0,0};	// 股関節横方向書込用 For hip joint lateral writing
double state_HW [2]={0,0};	// 膝関節書込用 For knee joint writing
double state_A0W[2]={0,0};	// 足首上下方向書込用 For writing in the upper and lower direction of the ankle
double state_A1W[2]={0,0};	// 足首横方向書込用 For ankle lateral writing
double state_U0W[2]={0,0};	// 肩前後方向書込用 For writing in shoulder anteroposterior direction
double state_U1W[2]={0,0};	// 肩横後方向書込用 For shoulder horizontal and posterior writing
double state_U2W[2]={0,0};	// 肩ヨー向書込用 For writing in shoulder yaw direction

// センサ関連 Sensor related
double state_fbRad=0;			// 頭前後角度 head front and back angle
double state_lrRad=0;			// 頭左右角度 head left and right angle
double state_fbAV=0;			// 頭前後角速度 head front and rear angular velocity
double state_lrAV=0;			// 頭左右角速度 Head left and right angular velocity
double state_asiPress_r=0;	// 右足裏圧力 right foot pressure
double state_asiPress_l=0;	// 左足裏圧力 left foot pressure

// 各種変数定義 Various variable definitions
double g_softERP;			// 柔らかさ、沈み込み softness, sinking
double g_softCFM;			// 柔らかさ、弾力 softness, elasticity
double g_bounce;			// 反発係数 coefficient of repulsion
double g_bounce_vel;		// 反発最低速度 minimum bounce speed

static dWorldID world;				// 動力学計算用ワールド World for dynamics calculation
static dSpaceID world_space;				// 衝突検出用スペース Space for collision detection
static dJointGroupID world_contactgroup;	// コンタクトグループ Contact group
static dGeomID world_ground;				// 地面 ground

dMatrix3 world_R;
const double* temp_Rot;		// 回転行列取得 Get rotation matrix
int		input_uvcOff=0;		// UVC起動フラグ UVC activation flag
unsigned char input_walkF=0;	// 歩行フラグ	（b0:歩行  b1:未  b2:未）walk flag (b0:walk b1:not yet b2:not yet)
int		world_bodyCount;		// ボディ配列カウント値 Body array count value
int		world_jointCount;		// ジョイント配列カウント値 Joint array count value
static struct dJointFeedback feedback[50];	// ジョイントフィードバック構造体 Joint feedback structure
double	input_frRatI;			// 上体角補正用積分係数 Integral coefficient for body angle correction
double	input_frRatA;			// 上体角オフセット値 Upper body angle offset value
double	input_fhRat;			// 足上げ高さ補正値 Foot lift height correction value
double	input_fwMax;			// 歩幅最大値 Maximum stride length


//###############  各種構造体 Various structures ###############
bodyStr *world_body[50];	// bodyStrアドレス格納配列 bodyStr address storage array
bodyStr biped_solep_r;	// 足裏圧力センサ Sole pressure sensor
bodyStr biped_solep_l;	
bodyStr biped_sole_r;	// 足裏 sole
bodyStr biped_sole_l;	
bodyStr biped_A1_r;		// 足首ロール	 ankle roll
bodyStr biped_A1_l;	
bodyStr biped_A0_r;		// 足首ピッチ ankle pitch
bodyStr biped_A0_l;
bodyStr biped_S_r;		// 脛 shin
bodyStr biped_S_l;
bodyStr biped_H_r;		// 膝 knees
bodyStr biped_H_l;
bodyStr biped_M_r;		// 腿 leg
bodyStr biped_M_l;
bodyStr biped_K0_r;		// 股関節ピッチ hip pitch
bodyStr biped_K0_l;
bodyStr biped_K1_r;		// 股関節ロール hip roll
bodyStr biped_K1_l;
bodyStr biped_DOU;		// 胴 torso
bodyStr biped_HEADT;	// 頭 head
bodyStr platform_DAI;		// 台 platform
bodyStr platform_DAI2;		// 台 platform
bodyStr barrier_base;		// 遮断機柱 barrier pillar
bodyStr barrier_pole;		// 遮断機棒 barrier rod

jointStr *world_joint[50];// jointStrアドレス格納配列 jointStr address storage array
jointStr biped_soleJ_r;	// 足裏センサ sole sensor
jointStr biped_soleJ_l;
jointStr biped_A1J_r;		// 足首ロール Ankle roll
jointStr biped_A1J_l;
jointStr biped_A0J_r;		// 足首ピッチ Ankle pitch
jointStr biped_A0J_l;
jointStr biped_SJ_r;		// 脛固定 Shin fixation
jointStr biped_SJ_l;
jointStr biped_HJ_r;		// 膝 knee
jointStr biped_HJ_l;
jointStr biped_MJ_r;		// 腿結合 Thigh joint
jointStr biped_MJ_l;
jointStr biped_M2J_r;		// 腿結合2 Thigh joint 2
jointStr biped_M2J_l;
jointStr biped_K0J_r;		// 股関節ピッチ Hip joint pitch
jointStr biped_K0J_l;
jointStr biped_K1J_r;		// 股関節ロール Hip roll
jointStr biped_K1J_l;
jointStr biped_K2J_r;		// 股関節ヨー Hip joint yaw
jointStr biped_K2J_l;
jointStr biped_HEADJ;		// 頭固定 Head fixation
jointStr platform_DAIJ;		// 台の固定 Fixing the stand
jointStr platform_DAI2J;		// 台2の固定 Fixing stand 2
jointStr barrier_baseJ;		// 柱の固定 Fixing the column
jointStr barrier_poleJ;		// ポールのジョイント pole joint

//###############  クラスの実体化　class instantiation ###############
core g_co;			// 歩行制御クラスの実体化 Instantiation of gait control class


//--------------------------------- command ----------------------------------------
static void command (int cmd){
	static int mag = 30;

	switch (cmd) {
		//// 外力印加 External input ////
		case 'j':case 'J':
			printf("F<-\n"); // Ｆ←\n
			dBodyAddForce(biped_DOU.bodyId, -20,0,0);
			break;
		case 'k':case 'K':
			printf("F->\n"); // Ｆ→\n
			dBodyAddForce(biped_DOU.bodyId,  20,0,0);
			break;
		case 'p':case 'P':
			printf("F^\n"); // Ｆ↑
			dBodyAddForce(biped_DOU.bodyId, 0,20,0);
			break;
		case 'l':case 'L':
			printf("F.\n"); // Ｆ↓
			dBodyAddForce(biped_DOU.bodyId,  0,-20,0);
			break;

		//// 操作 operate ////
		case 'w':				// 歩行開始 Start walking
			printf("Start walking\n"); // 歩行開始
			input_walkF=0x01;
			barrier_poleJ.t_jointAngle=5;
			break;

		case '1':				//外力実験 External force experiment
			printf("External force experiment\n"); // 外力実験
			restart ();
			input_fwMax=30;
			input_frRatA=0;
			input_frRatI=0;
			input_fhRat=0;
			setBody  (&barrier_base,			'y','d',	260,	 0,	  0,	24,		0,	180,	110,		0,	0.01);	//遮断機棒
			setBody  (&barrier_pole,			'y','y',	320,	 0,	  0,	8,		0,	300,	210,		1,	0.0001);	//ボール
			dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
			dBodySetRotation(barrier_pole.bodyId, world_R);
			setJoint(&barrier_baseJ,			'g',	&barrier_base,	&barrier_base,	'x',		0,	180,	0);		//遮断機柱固定用
			setJoint(&barrier_poleJ,			'h',	&barrier_pole,	&barrier_base,	'z',		0,	180,	210);	//遮断機棒ヒンジ
			barrier_poleJ.tm=1;
			barrier_poleJ.torque_tk=0.25;
			break;

		case '2':				// 前進実験 forward experiment
			printf("Forward experiment\n"); // 前進実験
			restart ();
			input_fwMax=30;
			input_frRatA=0.0015;
			input_frRatI=0;
			input_fhRat=0;
			goto saka;
			
		case '3':				// 登坂実験 Climbing experiment
			printf("Climbing experiment\n"); // 登坂実験
			restart ();
			input_fwMax=30;
			input_frRatA=0.0015;
			input_frRatI=0.2;
			input_fhRat=0;
			goto saka;

		case '4':				// 段差実験 step experiment
			printf("Step experiment\n"); // 段差実験
			restart ();
			input_fwMax=30;
			input_frRatA=0.0015;
			input_frRatI=0.2;
			input_fhRat=7;
			setBody  (&platform_DAI2,		'b','g',500,300,	 20,0,		0,	0,	-10,		 1,	100);	//水平台
			setJoint(&platform_DAI2J,		'g',	&platform_DAI2,&platform_DAI2,	'x',	50,	0,	0);						//台固定用
			break;

		case '5':				// 最終実験 final experiment
			printf("Final experiment\n"); // 最終実験
			restart ();
			input_fwMax=30;
			input_frRatI=0.2;
			input_frRatA=0.0015;
			input_fhRat=7;
saka:	// slope/hill
			setBody  (&platform_DAI,			'b','g',680,300,	 100,   0,	 480,	0,   -70,	1,	100);		//傾斜台
			dRFromAxisAndAngle(world_R, 0, 1, 0, -0.05);// 回転 rotate
			dBodySetRotation(platform_DAI.bodyId, world_R);
			setJoint(&platform_DAIJ,			'g',	&platform_DAI,		&platform_DAI,	'x',	  50,	0,	   0);				//台固定用
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
			if(input_uvcOff==0){
				input_uvcOff=1;
			}
			else{
				input_uvcOff=0;
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
			if(0 != barrier_pole.bodyId &&(b1==barrier_pole.bodyId||b2==barrier_pole.bodyId))	contact[i].surface.mu		= 0.2;			// ポールの摩擦 pole friction
			else									contact[i].surface.mu		= 5;			// 地面間摩擦 friction between floors
			contact[i].surface.bounce		= 0;												// bouncing the objects
			contact[i].surface.bounce_vel	= 0;												// bouncing velocity
			dJointID c = dJointCreateContact (world,world_contactgroup,&contact[i]);					// ジョイント生成 Joint generation
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
	for (i=0;i<world_jointCount;i++){
		switch (world_joint[i]->jointType) {
			case 'h':
				k = kp * (world_joint[i]->t_jointAngle - dJointGetHingeAngle(world_joint[i]->jointId));
				if(abs(k) > world_joint[i]->tm){
					if(k > 0){k = world_joint[i]->tm;}
					else{k = -world_joint[i]->tm;}
				}
				dJointSetHingeParam(world_joint[i]->jointId, dParamVel, k );
				dJointSetHingeParam(world_joint[i]->jointId, dParamFMax, world_joint[i]->torque_tk); 
				break;
			case 'd':
				k = world_joint[i]->t_jointAngle - dJointGetSliderPosition(world_joint[i]->jointId);
				dJointSetSliderParam(world_joint[i]->jointId, dParamVel,  k * 100);
				dJointSetSliderParam(world_joint[i]->jointId, dParamFMax, 300); 
				break;
		}
	}
}


//--------------------------------- simLoop ----------------------------------------
//	simulation loop
static void simLoop (int pause){
	int i;
	char inputKey;
	static int mag = 3;

	double sides[3];
	dJointFeedback *fb;
	dVector3 headVel1;
	dVector3 headVel2;

//	Sleep(1);			// 描画速度の調整 Adjusting drawing speed
	if(_kbhit()){
		inputKey=getchar();	// キー読込 key reading
		command (inputKey);
	}

	if (!pause) {
		//******** この３行は最初に置くべし These three lines should be placed first ********
		dSpaceCollide (world_space,0,&nearCallback);	// 衝突しそうなジオメトリのペア集団を探す Find pairs of geometries that are likely to collide
		dWorldStep (world,0.01);				// シミュレーションを１ステップ指定時間進める Advance the simulation by one step for the specified time
		dJointGroupEmpty (world_contactgroup);		// ジョイントグループを空にする Empty the joint group

		//******** 足裏圧力検出 Sole pressure detection ********
		fb = dJointGetFeedback(biped_soleJ_r.jointId);
		state_asiPress_r = fb->f1[2];				// 右足(足首Ｚ)圧力 Right foot (ankle Z) pressure
		fb = dJointGetFeedback(biped_soleJ_l.jointId);
		state_asiPress_l = fb->f1[2];				// 左足(足首Ｚ)圧力 Left foot (ankle Z) pressure

		//******** 頭前後左右角度検出 Head front, back, left, right, and right angles ********
		temp_Rot = dBodyGetRotation(biped_HEADT.bodyId);		// 回転行列取得 Get rotation matrix
		state_fbRad = asin(temp_Rot[8]);					// 頭前後角度(後ろに仰向きが正) Anteroposterior angle of the head (positive when facing backwards)
		state_lrRad = asin(temp_Rot[9]);					// 頭左右角度（右傾きが正） Head left/right angle (right tilt is positive)

		//******** 頭前後左右角速度検出 Head front, rear, left and right angular velocity ********
		temp_Rot = dBodyGetAngularVel(biped_HEADT.bodyId);	// 回転行列取得 Get rotation matrix
		state_fbAV = temp_Rot[1];						// 頭前後角度(後ろに仰向きが負) Head anteroposterior angle (negative when facing backwards)
		state_lrAV = temp_Rot[0];						// 頭左右角度（右傾きが正） Head left/right angle (right tilt is positive)

		biped_K0J_r.t_jointAngle	=state_K0W[0];			// 股関節前後方向書込用 For writing in hip joint anteroposterior direction
		biped_K1J_r.t_jointAngle	=state_K1W[0];			// 股関節横方向書込用 For hip joint lateral writing
		biped_HJ_r.t_jointAngle	=state_HW [0];			// 膝関節書込用 For writing knee joint
		biped_A0J_r.t_jointAngle	=state_A0W[0];			// 足首上下方向書込用 For writing in ankle up and down direction
		biped_A1J_r.t_jointAngle	=state_A1W[0];			// 足首横方向書込用 For ankle lateral writing

		biped_K0J_l.t_jointAngle	=state_K0W[1];			// 股関節前後方向書込用 For writing in hip joint anteroposterior direction
		biped_K1J_l.t_jointAngle	=state_K1W[1];			// 股関節横方向書込用 For hip joint lateral writing
		biped_HJ_l.t_jointAngle	=state_HW [1];			// 膝関節書込用  For writing knee joint
		biped_A0J_l.t_jointAngle	=state_A0W[1];			// 足首上下方向書込用 For writing in ankle up and down direction
		biped_A1J_l.t_jointAngle	=state_A1W[1];			// 足首横方向書込用 For ankle lateral writing

		g_co.walk();				// 歩行制御 Walk control
		control();				// モータ駆動 motor drive
	}

	for (i=0;i<world_bodyCount;i++){
		switch (world_body[i]->color) {
			case 'g':
				dsSetColor (0,1,0);
				break;
			case 'r':
				dsSetColor (1,0,0);
				break;
			case 'b':
				if(input_uvcOff==0)	dsSetColor(0.3 ,0.3, 2.0);			// 青 green
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
		switch (world_body[i]->bodyType) {
			case 'b':
				sides[0] = world_body[i]->length; sides[1] = world_body[i]->width; sides[2] = world_body[i]->height;
				dsDrawBox (dBodyGetPosition(world_body[i]->bodyId),dBodyGetRotation(world_body[i]->bodyId),sides);						//箱形表示
				break;
			case 's':
	 			dsDrawSphere (dBodyGetPosition(world_body[i]->bodyId),dBodyGetRotation(world_body[i]->bodyId),world_body[i]->radius);				//球形表示
				break;
			case 'c':
				dsDrawCapsule (dBodyGetPosition(world_body[i]->bodyId),dBodyGetRotation(world_body[i]->bodyId),world_body[i]->length,world_body[i]->radius);	//カプセル形表示
				break;
			case 'y':
				dsDrawCylinder (dBodyGetPosition(world_body[i]->bodyId),dBodyGetRotation(world_body[i]->bodyId),world_body[i]->length,world_body[i]->radius);	//円柱形表示
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
	b-> bodyType = k;			// ボディの種類を記録 Record body type
	b-> color = c;			// ボディの色の種類を記録 Record body color type
	b-> length = l;			// ボディの長さを記録 Record body length
	b-> width = w;			// ボディの幅さを記録 Record the width of the body
	b-> height = h;			// ボディの高さを記録 Record body height
	b-> radius = r;			// ボディの半径を記録 record body radius
	b-> hasGeometry = ge;			// ジオメトリ設定 有/無 Geometry settings Yes/No
	b-> enabled = 1;			// ボディ有効設定 Body enable setting

	x += 2;				// 2m手前に置いて地面障害物を避ける Place 2m in front to avoid ground obstacles

	world_body[world_bodyCount] = b;	// 構造体のアドレスを格納する Store address of structure
	++world_bodyCount;			// ボディ数カウントアップ body count up

	// ボディとジオメトリの生成と、質量、位置、関連性を設定 Generate bodies and geometry and set mass, position, and relationships
	switch (b->bodyType) {
		case 'b':	// 箱型 Box-shaped
			b->bodyId = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Generate the existence of an object (Body ID setting)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetBoxTotal (&m,ma,b->length,b->width,b->height);		// 物体の重量設定 Setting the weight of an object
			dBodySetMass (b->bodyId,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->bodyId,x,y,(z));				// 物体の初期位置 initial position of object
			if(ge > 0){
				b->geometry = dCreateBox (world_space,b->length,b->width,b->height);	// 物体の幾何情報を生成（ジオメトリID設定） Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->geometry,b->bodyId);					// 物体の『存在』と『幾何情報』の一致 Matching the “existence” of an object and its “geometric information”
			}
			break;
		case 's':	// 球形 spherical
			b->bodyId = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) create object (set body id)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetSphereTotal (&m,ma,b->radius);				// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->bodyId,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->bodyId,x,y,z);					// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->geometry = dCreateSphere (world_space,b->radius);			// 物体の幾何情報を生成（ジオメトリID設定） generate geometric information
				dGeomSetBody (b->geometry,b->bodyId);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
			}
			break;
		case 'c':	// カプセル形 capsule shape
			b->bodyId = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Create existence of object (set body ID
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetCapsuleTotal(&m,ma,3,b->radius,b->length);		// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->bodyId,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->bodyId,x,y,(b->length/2+z));			// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->geometry = dCreateCapsule (world_space,b->radius,b->length);	// 物体の幾何情報を生成（ジオメトリID設定） Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->geometry,b->bodyId);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
			}
			break;
		case 'y':	// 円柱形 cylindrical
			b->bodyId = dBodyCreate (world);						// 物体の存在を生成(ボディID設定) Create existence of object (set body ID)
			dMassSetZero(&m);								// 質量パラメータ初期化 Mass parameter initialization
			dMassSetCylinderTotal(&m,ma,3,b->radius,b->length);		// 物体の重量設定 Setting the weight of the object
			dBodySetMass (b->bodyId,&m);							// 物体の重量分布設定 Object weight distribution settings
			dBodySetPosition (b->bodyId,x,y,(z));				// 物体の初期位置 Initial position of the object
			if(ge > 0){
				b->geometry = dCreateCylinder (world_space,b->radius,b->length);	// 物体の幾何情報を生成（ジオメトリID設定）Generate geometric information of object (geometry ID setting)
				dGeomSetBody (b->geometry,b->bodyId);					// 物体の『存在』と『幾何情報』の一致 Match between the object's "existence" and "geometric information"
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
	j -> jointType	= k;			// 種類を記録 record type
	j -> origin_x	= x;			// X座標を記録 record the x coordinate
	j -> origin_y	= y;			// X座標を記録 record the y coordinate
	j -> origin_z	= z;			// X座標を記録 record the z coordinate
	j -> c_counter	= 0;			// 汎用カウンタ General purpose counter
	j -> t_jointAngle	= 0;			// 間接角度 indirect angle
	j -> t2_jointAngle	= 0;			// 間接角度2 indirect angle 2
	j -> mode = 0;			// 駆動モード drive mode
	j -> pn_footPressureCounter	= 0;			// 足圧力カウンタ foot pressure counter
	j -> tm	= 44.06;		// 8.06最大角速度 -- 8.06 maximum angular velocity
	j -> tm2	= 8.06;		// 8.06最大角速度 -- //8.06 maximum angular velocity
	j -> torque_tk	= 2.45;		// 2.45トルク 25kgfcm   (25/100)/9.8=2.45 -- 2.45 torque 25kgfcm (25/100)/9.8=2.45
	j -> torque_tk2	= 2.45;		// トルク2 torque 2

	x += 2;					// 2m手前に置いて地面障害物を避ける Place 2m in front to avoid ground obstacles
	world_joint[world_jointCount] = j;	// 配列のアドレスを格納する Storing the address of the array
	++world_jointCount;			// 配列カウンタ、カウントアップ array counter, count up

	switch (k) {
		case 'h':	// ヒンジジョイント hinge joint
			j -> jointId = dJointCreateHinge(world, 0);			// ヒンジジョイントの生成と記録 generate and record hinge joints
			dJointAttach(j -> jointId, b1->bodyId, b2->bodyId);				// ヒンジジョイントの取付 Installation of hinge joint
			dJointSetHingeAnchor(j -> jointId, x, y, z);			// 中心点の設定 Setting the center point
			switch (a) {		// 軸を設定 set axis
				case 'x': dJointSetHingeAxis(j -> jointId, 1, 0, 0); break;	// X軸の設定 set X axis
				case 'z': dJointSetHingeAxis(j -> jointId, 0, 0, 1); break;	// Z軸の設定 set Z axis
				default : dJointSetHingeAxis(j -> jointId, 0, 1, 0); break;	// Y軸の設定 set Y axis
			}
			break;
		case 'd':	// スライダージョイント（ダンパー） slider joint (damper)
			j -> jointId = dJointCreateSlider(world, 0);		// スライダージョイントの生成と記録 Generating and recording slider joints
			dJointAttach(j -> jointId, b1->bodyId, b2->bodyId);			// スライダージョイントの取付 Installing the slider joint
			dJointSetSliderAxis(j -> jointId, 0, 0, 1);		// 中心点の設定 Setting the center point
			break;
		case 'f':	// 固定ジョイント fixed joint
			j -> jointId = dJointCreateFixed(world, 0);		// 固定ジョイントの生成と記録 Generate and record fixed joints
			dJointAttach(j -> jointId, b1->bodyId, b2->bodyId);			// 固定ジョイントの取付 Installing the fixed joint
			dJointSetFixed(j -> jointId);						// 固定ジョイントにの設定 Setting to fixed joint
			break;
		case 'g':	// 環境固定ジョイント environment fixed joint
			j -> jointId = dJointCreateFixed(world, 0);		// 固定ジョイントの生成と記録 Generate and record fixed joints
			dJointAttach(j -> jointId, b1->bodyId, 0);				// 固定ジョイントの取付（環境固定） Installing a fixed joint (fixed environment)
			dJointSetFixed(j -> jointId);						// 固定ジョイントにの設定 Setting to fixed joint
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

	g_softERP		= 0.2;		// 弾力 Elasticity
	g_softCFM		= 0.0001;	// 沈み込み sinking
	g_bounce		= 0.01;		// 反発係数 coefficient of restitution
	g_bounce_vel	= 0.02;		// 反発最低速度 minimum bounce speed
	world_bodyCount	= 0;			// ボディ配列カウント値 body array count value
	world_jointCount	= 0;			// ジョイント配列カウント値 joint array count

//	####################
//	#### ボディ生成 body generation ####
//	####################
//						    種類 色		L　 W	H   R		X	Y	Z	ジオメト 重量
//                         Type Color   L   W   H   R       X   Y   Z  Geometry Weight

	setBody  (&biped_HEADT,		'c','w',	15,	0,	0,	21,		0,	0,	340,	0,	0.16);	// 頭 head
	setBody  (&biped_DOU,			'b','b',	40, 84, 130,0,		0,	0,	260,	1,	1.24);	// 胴 torso
	setBody  (&biped_K0_r,		'y','d',	34,	0,	0,	12,		0,	-fw,195,	0,	0.05);	// 股関節ピッチ hip pitch
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_K0_r.bodyId, world_R);
	setBody  (&biped_K0_l,		'y','d',	34,	0,	0,	12,		0,	fw,	195,	0,	0.05);
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_K0_l.bodyId, world_R);
	setBody  (&biped_K1_r,		'y','w',	34,	0,	0,	12,		0,	-fw,195,	0,	0.05);	// 股関節ロール hip roll
	dRFromAxisAndAngle(world_R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_K1_r.bodyId, world_R);
	setBody  (&biped_K1_l,		'y','w',	34,	0,	0,	12,		0,	fw,	195,	0,	0.05);
	dRFromAxisAndAngle(world_R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_K1_l.bodyId, world_R);
	setBody  (&biped_M_r,			'b','d',	20,	26,	90,	0,		0,	-fw,150,	0,	0.08);	// 腿 leg
	setBody  (&biped_M_l,			'b','d',	20,	26,	90, 0,		0,	fw,	150,	0,	0.08);
	setBody  (&biped_H_r,			'y','d',	34,	0,	0,  12,		0,	-fw,105,	0,	0.03);	// 膝 knees
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_H_r.bodyId, world_R);
	setBody  (&biped_H_l,			'y','d',	34,	0,	0,	12,		0,	fw,	105,	0,	0.03);
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_H_l.bodyId, world_R);
	setBody  (&biped_S_r,			'b','d',	20,	26, 90,	0,		0,	-fw,60,		1,	0.04);	// 脛 shin
	setBody  (&biped_S_l,			'b','d',	20,	26, 90,	0,		0,	fw,	60,		1,	0.04);
	setBody  (&biped_A0_r,		'y','d',	34,	 0, 0,	12,		0,	-fw,15,		0,	0.02);	// 足首ピッチ ankle pitch
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_A0_r.bodyId, world_R);
	setBody  (&biped_A0_l,		'y','d',	34,	 0,	0,	12,		0,	fw,	15,		0,	0.02);
	dRFromAxisAndAngle(world_R, 1, 0, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_A0_l.bodyId, world_R);
	setBody  (&biped_A1_r,		'y','w',	34,	 0,	0,	12,		0,	-fw,15,		0,	0.02);	// 足首ロール ankle roll
	dRFromAxisAndAngle(world_R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_A1_r.bodyId, world_R);
	setBody  (&biped_A1_l,		'y','w',	34,	 0,	0,	12,		0,	fw,	15,		0,	0.02);
	dRFromAxisAndAngle(world_R, 0, 1, 0, -M_PI_2);// 回転 rotate
	dBodySetRotation(biped_A1_l.bodyId, world_R);
	setBody  (&biped_sole_r,		'b','w',	55,	 40,2,	0,		0,	-fw,6.0,	0,	0.01);	// 足平 foot
	setBody  (&biped_sole_l,		'b','w',	55,	 40,2,	0,		0,  fw,	6.0,	0,	0.01);
	setBody  (&biped_solep_r,		'b','r',	55,	 40,6,	0,		0,	-fw,3.0,	1,	0.01);	// ソールセンサ sole sensor
	setBody  (&biped_solep_l,		'b','r',	55,	 40,6,	0,		0,  fw,	3.0,	1,	0.01);
	barrier_pole.bodyId=0;// 摩擦係数設定条件 Friction coefficient setting conditions

//	######################
//	####ジョイント生成 Joint generation ####
//	######################
//							種類		B番号1		B番号2	軸			X		Y		Z
//                        Type   B number 1 B number 2 Axis         X       Y       Z

	setJoint(&biped_HEADJ,		'f',	&biped_HEADT,		&biped_DOU,	'z',		0,		0,		360);	// 頭固定用 For head fixation
	setJoint(&biped_K0J_r,		'h',	&biped_K1_r,		&biped_K0_r,	'y',		0,		-fw,	195);	// 股関節ピッチ hip joint pitch
	setJoint(&biped_K0J_l,		'h',	&biped_K1_l,		&biped_K0_l,	'y',		0,		fw,		195);
	setJoint(&biped_K1J_r,		'h',	&biped_DOU,		&biped_K1_r,	'x',		0,		-fw+11,	195);	// 股関節ロール hip roll
	setJoint(&biped_K1J_l,		'h',	&biped_K1_l,		&biped_DOU,	'x',		0,		fw-11,	195);
	setJoint(&biped_MJ_r,			'f',	&biped_M_r,		&biped_K0_r,	'y',		0,		-fw,	128);	// 腿固定用 for leg fixation
	setJoint(&biped_MJ_l,			'f',	&biped_M_l,		&biped_K0_l,	'y',		0,		fw,		128);
	setJoint(&biped_M2J_r,		'f',	&biped_H_r,		&biped_M_r,	'y',		0,		-fw,	128);	// 腿固定用 for leg fixation
	setJoint(&biped_M2J_l,		'f',	&biped_H_l,		&biped_M_l,	'y',		0,		fw,		128);
	setJoint(&biped_HJ_r,			'h',	&biped_S_r,		&biped_H_r,	'y',		0,		-fw,	105);	// 膝関節 Knee fixation
	setJoint(&biped_HJ_l,			'h',	&biped_S_l,		&biped_H_l,	'y',		0,		fw,		105);
	setJoint(&biped_SJ_r,			'f',	&biped_S_r,		&biped_A0_r,	'y',		0,		-fw,	60);	// 脛固定用 For shin fixation
	setJoint(&biped_SJ_l,			'f',	&biped_S_l,		&biped_A0_l,	'y',		0,		fw,		60);
	setJoint(&biped_A0J_r,		'h',	&biped_A0_r,		&biped_A1_r,	'y',		0,		-fw,	15);	// 足首ピッチ Ankle pitch
	setJoint(&biped_A0J_l,		'h',	&biped_A0_l,		&biped_A1_l,	'y',		0,		fw,		15);
	setJoint(&biped_A1J_r,		'h',	&biped_A1_r,		&biped_sole_r,'x',		0,		-fw+11,	15);	// 足首ロール Ankle roll
	setJoint(&biped_A1J_l,		'h',	&biped_sole_l,	&biped_A1_l,	'x',		0,		fw-11,	15);
	setJoint(&biped_soleJ_r,		'd',	&biped_solep_r,	&biped_sole_r,'x',		0,		-fw,	6);		// ソール圧力センサ sole pressure sensor
	setJoint(&biped_soleJ_l,		'd',	&biped_solep_l,	&biped_sole_l,'x',		0,		fw,		6);

	dJointSetFeedback(biped_soleJ_r.jointId,			&feedback[0]);
	dJointSetFeedback(biped_soleJ_l.jointId,			&feedback[1]);
}


//--------------------------------- destroy ----------------------------------------
//	ロボットの破壊 robot destruction
void destroyBot (){
	int i;
	for (i=0;i<world_jointCount;i++){
		if(world_joint[i]->enabled > 0){dJointDestroy (world_joint[i]->jointId);}	// ジョイント破壊 Joint destruction
	}
	for (i=0;i<world_bodyCount;i++){
		if(world_body[i]->enabled > 0){dBodyDestroy (world_body[i]->bodyId);}		// ボディ有効なら破壊 Destroy if body is valid
		if(world_body[i]->hasGeometry > 0){dGeomDestroy (world_body[i]->geometry);}		// ジオメトリ設定されてたら破壊 Destroyed if geometry is set
	}
	dJointGroupDestroy (world_contactgroup);
}


//--------------------------------- restart ----------------------------------------
//	リスタート restart
void restart (){
	destroyBot ();
	world_contactgroup = dJointGroupCreate (0);	// 接触点のグループを格納する入れ物 A container that stores a group of contact points
	createBody();						// ロボット生成 robot generation
	dWorldSetGravity (world, 0, 0, -9.8);	// 重力設定 Gravity settings
	g_co.mode=0;
	g_co.autoHs=180;
	input_walkF=0;
	input_uvcOff=0;
	input_frRatI=0;
	input_frRatA=0;
	input_fhRat=0;
	input_fwMax=0;
 
	state_K0W[0]=0;			//股関節前後方向 hip joint anteroposterior direction
	state_K1W[0]=0;			//股関節横方向 Hip joint lateral direction
	state_HW [0]=0;			//膝関節 knee joint
	state_A0W[0]=0;			//足首上下方向 Ankle up and down direction
	state_A1W[0]=0;			//足首横方向 Ankle lateral direction
	state_K0W[1]=0;			//股関節前後方向 hip joint anteroposterior direction
	state_K1W[1]=0;			//股関節横方向 Hip joint lateral direction
	state_HW [1]=0;			//膝関節 knee joint
	state_A0W[1]=0;			//足首上下方向 Ankle up and down direction
	state_A1W[1]=0;			//足首横方向 Ankle lateral direction
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
	world_space = dHashSpaceCreate (0);			// 衝突検出用スペース生成 Create space for collision detection
	world_contactgroup = dJointGroupCreate (0);	// 接触点のグループを格納する入れ物 Container for storing groups of contact points
	dWorldSetGravity (world, 0, 0, -9.8);	// 重力設定 Gravity settings
	world_ground = dCreatePlane (world_space,0,0,1,0);	// 平面のジオメトリ作成 Create plane geometry
	createBody();							// ロボット生成 Create robot

	// run simulation
	dsSimulationLoop (argc,argv,640,640,&fn);

	// 後始末 clean up
	destroyBot ();
	dSpaceDestroy (world_space);
	dWorldDestroy (world);
	return 0;
}