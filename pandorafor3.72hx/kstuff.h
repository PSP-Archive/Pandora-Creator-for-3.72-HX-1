int kstuffInitBuffers(u8 *b1, u8 *b2, u8 *b3, char *b4);
int kstuffMsInstallFW(void);
int kstuffMsInstallAdditional(void);
int kstuffBatMakeService(void);
int kstuffBatMakeNormal(void);
void kstuffSetUserFileWriteFunc(int* func);
int kstuffGetMsPartitionStart(void);
