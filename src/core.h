typedef struct {
	char* dataFlags;
	char* dataDescription;
	int count_flags;
} stData;

#ifdef __cplusplus
extern "C" {
#endif

void GetOptions(stData* arrData, char* pCommand);

#ifdef __cplusplus
}
#endif
