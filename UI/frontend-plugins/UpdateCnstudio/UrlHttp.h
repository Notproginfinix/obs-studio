#ifndef __HTTPCLIENT___
#define __HTTPCLIENT___
typedef	int (*PROGRESSFUN)(void *p,double dltotal, double dlnow,double ultotal, double ulnow);
typedef int (*PROGRESSTHREADFUN)( void* lpVoid );
#define DEFCONNECTTIME 120
#define DEFRECVETIME 120

#define NEWURLDESKEY "i2.0908o"

class CHttpClient  
{  
public: 
	struct WriteThis {
		const char *readptr;
		long sizeleft;
	};
     CHttpClient(void);  
    ~CHttpClient(void);
	 /** 
     * @brief   ��ʼ��http������Դ
	 */
	 bool		InitHttpConnect();
	 /** 
     * @brief   �ر�http������Դ
	 */
	 void		 CloseHttpConnect();
	 CURL*		 GetLibCurl();
	/** 
     * @brief   ���ø��ӵ�����ͷ
	 */
	void		SetUserHeader(std::vector<std::string>* inHeader);
	/** 
     * @brief   ������ӵ�����ͷ
	 */
	void		ClearUserHeader();
	/** 
     * @brief   ����cookie·��
	 */
	void			SetCookiePatch(const std::string& strFile);
	void			SetFollowLocation(bool bLocation);
	std::string		GetHttpResHeader();
	std::string		GetLastUrl();
	std::string		GetLastConnectIp();
	CURLcode		ExecuteHttp(CURL* http);
	__int64			DownUrlToFileEx(const std::string & strUrl,const std::string& localFileName);
	__int64			GetFileLen(const std::string& file);
	/** 
    * @brief �õ�Զ���ļ���С
	*/
	 __int64  GetRmoteFileSize(const std::string & strUrl);
	 /** 
    * @brief  ����ָurl���ݵ������ļ�
	*/
	 FILE*    GetDownFilePtr();
	__int64	  DownUrlToFile(const std::string & strUrl,const std::string& localFileName,unsigned __int64 iBeg=0,PROGRESSFUN cb=NULL,void* userData=NULL);

	CURLcode  UpLoadFile(const std::string& strUrl,const std::string& localFileName,std::string &strResponse);
	//���ݾɰ�ӿ�
	void UpAddForm(curl_httppost** phttppost,curl_httppost** plastpost,const std::string& name,const std::string& var,bool bFile=true);
	CURLcode UpLoadForm(const std::string& strUrl,curl_httppost* phttppost,std::string& strResponse);
	/** 
    * @brief
	*/
	void     SetHttpTimeOut(DWORD dwConnectTime=DEFCONNECTTIME,DWORD dwRequestTime=DEFRECVETIME);
	/**
	* @bries ���ý��������Ƿ�ֻ����ͷ
	*/
	void     SetResponseHeader(bool bHeader=false);
    /** 
    * @brief HTTP POST���� 
    * @param strUrl		 �������,�����Url��ַ,��:http://www.xxxx.com 
    * @param strPost	 �������,��������
    * @param strResponse �������,���ص�����
    * @return			 �����Ƿ�Post�ɹ� 
    */  
	int		Post(const std::string & strUrl,bool bPostBuff, const std::string & strPost, std::string & strResponse);  
  
    /** 
    * @brief HTTP		 GET���� 
    * @param strUrl		 �������,�����Url��ַ,��:http://www.xxxx.com 
    * @param strResponse �������,���ص�����
    * @return			 �����Ƿ�Post�ɹ� 
    */  
    int		Get(const std::string & strUrl, std::string & strResponse);  
  
    /** 
    * @brief HTTPS POST  ����,��֤��汾 
    * @param strUrl		 �������,�����Url��ַ,��:https://www.xxxxx.com 
    * @param strPost	 �������,��������
    * @param strResponse �������,���ص�����
    * @param pCaPath	 �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��. 
    * @return			 �����Ƿ�Post�ɹ� 
    */  
    int		Posts(const std::string & strUrl,bool bPostBuff, const std::string & strPost, std::string & strResponse,const char * pCaPath = NULL);  
    /** 
    * @brief HTTPS       GET����,��֤��汾 
    * @param strUrl		 �������,�����Url��ַ,��:https://www.xxxx.com 
    * @param strResponse �������,���ص�����
    * @param pCaPath	 �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��. 
    * @return			 �����Ƿ�Post�ɹ� 
    */  
    int		Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);
	//��������
	/** 
    * @brief			 ����http����ͷ��ֵ
    * @param strKey		 ���صĴ�  
    */ 
	std::string	GetHttpHeaderVar(const std::string& strKey);
	std::string	UrlEncode(const std::string& strSrc);
	std::string	UrlDecode(const std::string& strSrc);
	std::string GetHttpHeaderDate();
	static size_t WriteFileCallback(void *contents, size_t size, size_t nmemb, void *userp);

public:  
    void		  SetDebug(bool bDebug);
	static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid);
	static size_t OnReadData(void *ptr, size_t size, size_t nmemb, void *userp);
	static int	  OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *);
	static size_t writeFunc( void* buffer, size_t size, size_t nmemb, void* lpVoid );
	
	CURLcode						 m_curlretcode;
	int							     m_httpretcode;
	bool					         m_bStop;
	static	CRITICAL_SECTION				 *lockarray;  
	
public:
	void  SetDefHeader(CURL* curl);
	void* ApplyUserHeader();
	void SetDecode( BOOL decode );
 private:
    bool							 m_bDebug;
	bool							 m_bLocation;
	std::string							 m_strHeader;
	void*							 m_mUserData;
	std::string							 m_mCookieFile;
	PROGRESSFUN						 m_progressCB;
	bool							 m_bRecvHeader;
	DWORD							 m_dwRequestTime;
	DWORD							 m_dwConnectTime;
	std::vector<const std::string>	 m_mVheader;
	public:
	CURL*							 m_curl;
	
	std::string						 m_qurl;
	FILE*							 m_FilePtr;
};  
#endif