!-----------------------------
! YAMLD ���������� ������ ������������ �� ����� � ������� ��� YAMLD
! dee2019-08-26
! dee2019-08-11
! dee2019-08-09
!-----------------------------
      include('base64.inc'),ONCE   !BASE64 ���������

YAMLD_MaxLenFLDFile  EQUATE(1024)  !������ ������ � �����
YAMLD_MaxLenFLD      EQUATE(512)   !������ ���� ��� ������� ������(���1/���2/���3)
YAMLD_MaxLenFLDValue EQUATE(2048)  !������ ���� ������������ ��������

YAMLD_cfgQ_TYPE     QUEUE,pre(YAMLD_cfgQ_TYPE),THREAD,TYPE  !����� �� ����� CFG YAML
id                  long                             !����� ������
Indent              long                             !��������
HeirsParent         long                             !������ ���������� ID
Parvalue            STRING(YAMLD_MaxLenFLDFile)      !�������� ���������
Node                string(YAMLD_MaxLenFLD)          !������� ������(���1/���2/���3)
Value               string(YAMLD_MaxLenFLDValue)     !�������� ���������
                    end
YAMLD_DimQ_TYPE     QUEUE,pre(YAMLD_DimQ_TYPE),THREAD,TYPE  !����������� dim
size                long                             !������
Node                string(YAMLD_MaxLenFLD)          !������� ������(���1/���2/���3)
                    end
                   
YAMLD               Class,Type,Module('YAMLD.clw'),Link('YAMLD.clw'),THREAD
YAMLD_cfgQ          &YAMLD_cfgQ_TYPE,protected
YAMLD_DimQ          &YAMLD_DimQ_TYPE,protected
YAMLD_SepFld        string(1),protected
YAMLD_Comment       string(1),protected
p_debug             byte,protected
Construct           Procedure(),Private
Destruct            Procedure(),Private
!---������� ������
YAMLD_Rcommand      Procedure(string _param),string                         !�������� �������� ��������� ��� �����
YAMLD_Scommand      Procedure(string _param, string _value),long,proc       !������ �������� ��������� ��� ����� RC=1 ������ RC=0 �� ������
YAMLD_GetIndex      procedure(string _param),long                           !�������� ������ ������� ��������� ����� "-"
YAMLD_init          procedure(String _InFile,<string _Sep>,<string _SComment>,byte _debug=0),long            !������������� ����� YAML � �������� ����������� �����
!---���������
YAMLD_GetFieldp     Procedure(String Stro, Short Num, String Sep, <Byte QuoteFlag>),String,Private
YAMLD_CfgLoadfile   Procedure(String _InFile),long,Private                  !��������� ���� � ������� YAML
YAMLD_CfgDeSerelize Procedure(),long,Private                                !1 ���� ��������� (��������� ������� ���������� � ������)
YAMLD_CfgHeirs      Procedure(),long,Private                                !1 ���� ��������� (��������� ������������)
YAMLD_B64D          Procedure(String _InData),string,Private                !base64 decode
!---�������
YAMLD_LoggerQ       Procedure(String _Message, *Queue _InQ),Private         !������� �������� Q
YAMLD_xIsType       Procedure(*? _Var),long,Private
YAMLD_xIsDimSize    Procedure(*? _Var),long,Private
                    end

!-------------------
! ���������:
! �������� � ������ ������ �������� �������
! ������ ���������� � ����� �������� �������� ��������� � ������� ���� � �������� ������ ������
! ��������� � ���������� �������� �������� ���: ����1/����3 ���������� �������� = ��������3
! ����������� ������� 
! ��� ���� ��������: 
!         & � *. ��� �������� ��������� ���������� ������ �� ������� � ����� ��� ������������.
!         &-������ ���� ����� �����������
!         *-� �������� ����� ������ � ��������� ������
!         |-������ ����� ���������� � ���� ������ (��� ���������� �������� ������)
!         |binary- base64 ������ ����� ���������� � ���� ������ � ������������
!         >-������ ����� ���������� � ���� ������ (� ����������� �������� ������)
!         {-������ Json ������ ���� {key1: "value1", key2: "value2"}
! ��� ����� �����:
!         !-� ����� ����� ����������� (��� ��� ������ ������������ ����������)
!         [..]-(� ������ ������) - ������ ������������ ����������
!         ---  (� ������ ������) - ������ ���������
!         ...  (� ������ ������) - ����� ���������
!
!����1 = ��������1     !��� �����������
!    ����2 = ��������2
!    ����3 = ��������3
!��� �����������
!����4 = ��������4
!����5 = &��������5    !������ ��� ������������
!    ����6 = ��������6
!    ����7 = ��������7
!����8 = ��������8
!����9 = *��������5    !������ �� ������������ ������
!����10 = ��������10
!����10 = |            !������ ����� ���������� � ���� ������ ������: ����10 = txttxttxttxttxttxt txttxt txttxt txttxttxttxt erte rte ttxttxt txttxt
!    txttxttxttxt
!    txttxt txttxt txttxt txttxt
!    txttxt erte rte t 
!    txttxt txttxt
!
!------�������� ����-----------
!! City = table
!ID = field
!   type = int 11
!   null = false
!   unique = true
!Name = field
!   type = char 35
!   null = true
!   Rama
!       zalman = true
!       zalman2 = false
!!--------dtetretretretet---------
!   unique = false
!   unique2 = off       !sdfsfsfsdf sdfsf
!     toper
!        siko1 = on
!        siko2 = of
!   unique3 = wite
!City
!  ID = 1
!  Name = \Kabul
!  ��������������������� = &basic
!        siko1 = on
!        siko2 = of1  
!        toper
!             zzzz = 555
!        siko2 = of
!        siko3 = of2
!        siko4 = of3
!  Stor = fff
!City
!  ID = 4079
!  Name = \Rafah
!City
!  ID = 23023
!  gruppa =     *basic
!  siko3 = of2_2
!  Name = \Moscow
!TestTXT
!   Blob = |
!     ertetretert
!     etretet rtyry tryut yu
!     rtert erte rte t 
!     ertet etret
!   rine = 45
!TestTXTRF
!   B64 = |binary
!     Ic/u5O/z7eryX+zg8fHo4jogWzEsIDIsIDNdDQ
!     ohDQoh0e/o8eroICjv7vHr5eTu4uDy5ev87e7x
!     8ugsIGxpc3RzLCBzZXF1ZW5jZXMsIGNvbGxlY3
!     Rpb25zKSDv8OXk8fLg4uv//vIg8e7h7ukg6u7r
!     6+Xq9ujoIA0KIfPv7vD/5O735e3t+/Ug5ODt7f
!     v1LCDk7vHy8+8g6iDq7vLu8PvsIOLu5+zu5uXt
!     IO/uIOj1IOjt5OXq8eDsLg0KIWJpbmRpbmdzOg
!     0KISAgLSBpcmNFdmVudDogUFJJVk1TRw0KISAg
!     ICBtZXRob2Q6IG5ld1VyaQ0KISAgICByZWdleH
!     A6ICdeaHR0cDovLy4qJw0KISAgLSBpcmNFdmVu
!     dDogUFJJVk1TRw0KISAgICBtZXRob2Q6IGRlbG
!     V0ZVVyaQ0KISAgICByZWdleHA6ICdeZGVsZXRl
!     LionDQohICAtIGlyY0V2ZW50OiBQUklWTVNHDQ
!     ohICAgIG1ldGhvZDogcmFuZG9tVXJpDQohICAg
!     IHJlZ2V4cDogJ15yYW5kb20uKicNCiEtLS0tLS
!     0tLS0tLS0tLS0tLS0tLS0tLS0t
!M800x600
!  CashrepGetVcode
!     MenuStyle = *MSTempl1
!     MenuScrollStyle = *MSSTempl1
!     MenuTypeID = 2
!     StrJson = {key1: "value1", key2: "value2"} 
!     323224 = 7777
!bindings
!  - 
!    ircEvent= PRIVMSG
!    method= newUri
!    regexp= '^http://.*'
!  - 
!    ircEvent= PRIVMSG
!    method= deleteUri
!    regexp= '^delete.*'
!  - 
!    ircEvent= PRIVMSG
!    method= randomUri
!    regexp= '^random.*'
!-------------------