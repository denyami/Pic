/*******************************************************************************
*  skI2Cslave - Ｉ２Ｃ通信を行うスレーブ用の関数ライブラリ                     *
*                                                                              *
*    InterI2C         - Ｉ２Ｃ関連の割り込み処理(この関数で実際に送受信する)   *
*    InitI2C_Slave    - Ｉ２Ｃ通信のスレーブモードで初期化を行う処理           *
*    I2C_ReceiveCheck - マスターからの受信状態をチェックする処理               *
*                                                                              *
*    メモ：SDA/SCLピンは必ず「デジタル入力ピン」に設定を行って下さい。         *
*          このI2C関数ライブラリは７ビットアドレスモードのみ対応です。         *
* ============================================================================ *
*  VERSION DATE        BY                    CHANGE/COMMENT                    *
* ---------------------------------------------------------------------------- *
*  1.00    2011-12-23  きむ茶工房(きむしげ)  Create                            *
* ============================================================================ *
*  PIC 12F1822 16F1827                                                         *
*  MPLAB IDE(V8.63)                                                            *
*  HI-TECH C Compiler for PIC10/12/16 MCUs Version 9.80 in Lite mode           *
*******************************************************************************/
#include <pic.h>
#include "skI2Cslave.h"       // I2C関数ライブラリー用

unsigned int  rcv_flg ;       // 受信情報(受信データの個数を格納)
unsigned char *Sdtp ;         // 送信データバッファのアドレスポインター
unsigned char *Rdtp ;         // 受信データバッファのアドレスポインター

/*******************************************************************************
*  interrupt InterI2C( void )                                                  *
*    Ｉ２Ｃ関連の割り込み処理                                                  *
*******************************************************************************/
void  interrupt InterI2C( void )
{
     char x ;

     /*********************************************************************
     * SSP(I2C)割り込み発生時の処理                                       *
     *********************************************************************/
     if (SSP1IF == 1) {  // 
          if (SSP1STATbits.R_nW == 0) {
               /******* マスターからの書き込み要求(スレーブは受信) *******/
               if (SSP1STATbits.D_nA == 0) {
                    // 受信バイトはアドレス
                    Rdtp = (char *)rcv_data ;
                    x = SSP1BUF ;       // アドレスデータを空読みする
                    rcv_flg = 0 ;
               } else {
                    // 受信バイトはデータ
                    *Rdtp = SSP1BUF ;   // データを読込む(ACKはPICが自動的に返す)
                    Rdtp++ ;
                    rcv_flg++ ;
               }
               SSP1IF = 0 ;             // 割込みフラグクリア
               SSP1CON1bits.CKP = 1 ;   // SCLラインを開放する(通信の再開)
          } else {
               /******* マスターからの読み出し要求(スレーブは送信) *******/
               if (SSP1STATbits.BF == 1) {
                    // アドレス受信後の割り込みと判断する
                    Sdtp = (char *)snd_data ;
                    x = SSP1BUF ;       // アドレスデータを空読みする
                    while((SSP1CON1bits.CKP)|(SSP1STATbits.BF)) ;
                    SSP1BUF = *Sdtp ;             // 送信データのセット
                    Sdtp++ ;
                    SSP1IF = 0 ;                  // 割込みフラグクリア
                    SSP1CON1bits.CKP = 1 ;        // SCLラインを開放する(通信の再開)
               } else {
                    // データの送信後のACK受け取りによる割り込みと判断する
                    if (SSP1CON2bits.ACKSTAT==0) {
                         // マスターからACK応答なら次のデータを送信する
                         while((SSP1CON1bits.CKP)|(SSP1STATbits.BF)) ;
                         SSP1BUF = *Sdtp ;        // 送信データのセット
                         Sdtp++ ;
                         SSP1IF = 0 ;             // 割込みフラグクリア
                         SSP1CON1bits.CKP = 1 ;   // SCLラインを開放する(通信の再開)
                    } else {
                         // マスターからはNOACKで応答された時
                         SSP1IF = 0 ;             // 割込みフラグクリア
                    }
               }
          }
     }
     /*********************************************************************
     * MSSP(I2C)バス衝突割り込発生時の処理                                *
     *********************************************************************/
     if (BCL1IF == 1) {
          BCL1IF = 0 ;   // 今回はフラグのみクリアする(無処理)
     }
}
/*******************************************************************************
*  InitI2C_Slave(address)                                                      *
*    Ｉ２Ｃ通信のスレーブモードで初期化を行う処理                              *
*     一括呼び出しの同報通知は無効の設定となっています。                       *
*                                                                              *
*    address : スレーブのマイアドレスを設定する(８〜１１９を推奨)              *
*******************************************************************************/
void InitI2C_Slave(int address)
{
     SSP1STAT= 0b10000000 ;   // 標準速度モードに設定する(100kHz)
     SSP1CON1= 0b00100110 ;   // SDA/SCLピンはI2Cで使用し、スレーブモードとする
     SSP1CON2bits.SEN  = 1;   // SCL制御(クロックストレッチ)を行う
//   SSP1CON2bits.GCEN = 1;   // 一括呼び出しの同報通知を有効にする
     SSP1ADD = address<<1 ;   // マイアドレスの設定
     SSP1MSK = 0b11111110 ;   // アドレス比較用マスクデータ
     SSP1IE = 1 ;             // SSP(I2C)割り込みを許可する
     BCL1IE = 1 ;             // MSSP(I2C)バス衝突割り込みを許可する
     PEIE   = 1 ;             // 周辺装置割り込みを許可する
     GIE    = 1 ;             // 全割り込み処理を許可する 
     SSP1IF = 0 ;             // SSP(I2C)割り込みフラグをクリアする
     BCL1IF = 0 ;             // MSSP(I2C)バス衝突割り込みフラグをクリアする
     rcv_flg = 0 ;
}
/*******************************************************************************
*  ans = I2C_ReceiveCheck()                                                    *
*    マスターからの受信状態をチェックする処理                                  *
*                                                                              *
*    ans  : 戻り値が０なら未受信で、受信したら受信したデータの個数を返す       *
*******************************************************************************/
int I2C_ReceiveCheck()
{
     int ans ;

     ans = 0 ;
     if (rcv_flg != 0) { // 受信したぞぉ
          if ((SSP1STATbits.S==0)&&(SSP1STATbits.P==1)) { // ｽﾄｯﾌﾟｺﾝﾃﾞｨｼｮﾝ発行されたぞぉ
               ans = rcv_flg ;
               rcv_flg = 0 ;
          }
     }
     return(ans) ;
}
