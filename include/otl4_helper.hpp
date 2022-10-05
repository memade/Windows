#if !defined(AFX_SK_H__72CCE109_9AE7_437C_8E40_CAEFF2CD332C__HEAD__)
#define AFX_SK_H__72CCE109_9AE7_437C_8E40_CAEFF2CD332C__HEAD__

#define OTL_ODBC_MSSQL_2005 // Compile OTL 4.0/SNAC 10 or later
#define OTL_ODBC_TIME_ZONE
#define _CRT_SECURE_NO_WARNINGS
#define OTL_STL // Turn on STL features
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts

#include "../gpl/otl4/otlv4.h"

namespace shared {
	namespace otl {

		class SqlServerUtil final {
		public:
			static std::string Replace(std::string& s, const std::string& toReplace, const std::string& replaceWith)
			{
				size_t pos;
				while ((pos = s.find(toReplace)) != std::string::npos)
					s.replace(pos, toReplace.length(), replaceWith);
				return s;
			}

			static std::string  ReadFile(const char* path)
			{
				std::ifstream t(path);
				return std::string((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
			}

			static void WriteFile(const char* path, std::string& content)
			{
				std::ofstream out(path);
				out << content;
				out.close();
			}

			static std::vector<std::string> Split(std::string stringToBeSplitted, std::string delimeter)
			{
				std::vector<std::string> splittedString;
				size_t startIndex = 0;
				size_t  endIndex = 0;
				while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
				{
					std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
					splittedString.push_back(val);
					startIndex = endIndex + delimeter.size();
				}
				if (startIndex < stringToBeSplitted.size())
				{
					std::string val = stringToBeSplitted.substr(startIndex);
					splittedString.push_back(val);
				}
				return splittedString;
			}

			//wstring高字节不为0，返回FALSE
			static bool WStringToString(const std::wstring wstr, std::string& str)
			{
#ifdef WIN32
				LPCWSTR ws = wstr.c_str();
				int wsLen = WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL);
				char* dst = new char[wsLen];
				int nResult = WideCharToMultiByte(CP_ACP, 0, ws, -1, dst, wsLen, NULL, NULL);
				if (nResult == 0)
				{
					delete[] dst;
					return false;
				}

				dst[wsLen - 1] = 0;
				str = dst;
				delete[] dst;
#endif
				return true;
			}

			static std::string AsciiCharPtrToUtf8(const char* pszOutBuffer)
			{
				size_t dwNum = ::MultiByteToWideChar(CP_ACP, 0, pszOutBuffer, -1, NULL, 0);    // 返回原始ASCII码的字符数目       
				wchar_t* pwText = new wchar_t[dwNum];                                       // 根据ASCII码的字符数分配UTF8的空间
				ZeroMemory(pwText, dwNum * 2 + 2);

				MultiByteToWideChar(CP_UTF8, 0, pszOutBuffer, -1, pwText, (int)dwNum);           // 将ASCII码转换成UTF8

				std::string retStr;

				WStringToString(pwText, retStr);

				delete[] pwText;
				pwText = NULL;

				return retStr;
			}
		};


		/// <summary>
/// where子句各个条件的组合方式
/// </summary>
		enum class SqlServerLogicalOperatorType : UINT
		{
			/// <summary>
			/// and
			/// </summary>
			AND = 0,
			/// <summary>
			/// or
			/// </summary>
			OR,
			/// <summary>
			/// 无效运算符
			/// </summary>
			NONE
		};

		/// <summary>
		/// where子句各个条件的条件判断方式
		/// </summary>
		enum class SqlServerConditionOperatorType : UINT 
		{
			/// <summary>
			/// is null
			/// </summary>
			IS_NULL,
			/// <summary>
			/// is not null
			/// </summary>
			NOT_NULL,
			/// <summary>
			/// =
			/// </summary>
			EQ,
			/// <summary>
			/// !=
			/// </summary>
			NOT_EQ,
			/// <summary>
			/// &lt;
			/// </summary>
			LT,
			/// <summary>
			/// &gt;
			/// </summary>
			GT,
			/// <summary>
			/// The less than or equal
			/// </summary>
			LE,
			/// <summary>
			/// The greater than or equal
			/// </summary>
			GE,
			/// <summary>
			/// in
			/// </summary>
			IS_IN,
			/// <summary>
			/// like
			/// </summary>
			LIKE,
			/// <summary>
			/// not in
			/// </summary>
			NOT_IN,
			/// <summary>
			/// not like
			/// </summary>
			NOT_LIKE
		};

		class SqlServerSqlParameter {
		public:
			// doc:http://otl.sourceforge.net/otl4_mssql_examples.htm
			enum class SqlParameterType :  UINT
			{
				STRING,
				DOUBLE,
				FLOAT,
				INT,
				UNSIGNED,
				LONG,
				TIMESTAMP,
				NONE,
			};

			std::string ParameterName;
			SqlParameterType ParameterType = SqlParameterType::STRING;
			int ParameterTypeSize = 0;

			SqlServerSqlParameter() {
			}

			SqlServerSqlParameter(const std::string& parameterName, const SqlParameterType& parameterType, const int& parameterTypeSize) 
			{
				this->ParameterName = parameterName;
				this->ParameterType = parameterType;
				this->ParameterTypeSize = parameterTypeSize;
			}
		};

		/// <summary>
		/// sql where子句中的条件
		/// </summary>
		class SqlServerCondition
		{
		private:
			std::vector<SqlServerCondition> Child;

		public:
			SqlServerLogicalOperatorType LogicalOperator = SqlServerLogicalOperatorType::AND;
			SqlServerConditionOperatorType ConditionOperator = SqlServerConditionOperatorType::EQ;
			std::string Sql = "";

			SqlServerSqlParameter Parameter;

			SqlServerCondition() {
			}

			SqlServerCondition(SqlServerLogicalOperatorType logicalOperator, std::string sql = "") {
				this->LogicalOperator = logicalOperator;
				this->Sql = sql;
			}

			SqlServerCondition(SqlServerLogicalOperatorType logicalOperator,
				SqlServerConditionOperatorType conditionOperator, SqlServerSqlParameter parameter) {
				this->LogicalOperator = logicalOperator;
				this->ConditionOperator = conditionOperator;
				this->Parameter = parameter;
			}

			void AddCondition(SqlServerCondition condition) {
				Child.push_back(condition);
			}

			void AddCondition(SqlServerLogicalOperatorType logicalOperator, std::string sql = "") {
				Child.push_back(SqlServerCondition(logicalOperator, sql));
			}

			void AddCondition(SqlServerLogicalOperatorType logicalOperator,
				SqlServerConditionOperatorType conditionOperator, SqlServerSqlParameter parameter) {
				Child.push_back(SqlServerCondition(logicalOperator, conditionOperator, parameter));
			}

			std::vector<SqlServerCondition>& GetChild() {
				return Child;
			}
		};


		class VarInfo {

		public:
			SqlServerSqlParameter::SqlParameterType Type;
			size_t Index;

			VarInfo(SqlServerSqlParameter::SqlParameterType type, size_t index) {
				this->Type = type;
				this->Index = index;
			}
		};

		class ConditionVars {

		private:
			std::vector<VarInfo> VarInfos;

			std::vector<int> Ints;
			std::vector<double> Doubles;
			std::vector<float> Floats;
			std::vector<unsigned int> UnsignedInts;
			std::vector<long> Longs;
			std::vector<otl_datetime> OtlDateTimes;
			std::vector<std::string> Strings;

		public:

			ConditionVars() {
			}

			void AddIntValue(int val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::INT, Ints.size()));
				Ints.push_back(val);
			}

			void AddDoubleValue(double val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::DOUBLE, Doubles.size()));
				Doubles.push_back(val);
			}

			void AddFloatValue(float val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::FLOAT, Floats.size()));
				Floats.push_back(val);
			}

			void AddUnsignedIntValue(unsigned int val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::UNSIGNED, UnsignedInts.size()));
				UnsignedInts.push_back(val);
			}

			void AddLongValue(long val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::LONG, Longs.size()));
				Longs.push_back(val);
			}


			void AddOtlDateTimeValue(otl_datetime val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::TIMESTAMP, OtlDateTimes.size()));
				OtlDateTimes.push_back(val);
			}

			void AddStringValue(std::string val) {
				VarInfos.push_back(VarInfo(SqlServerSqlParameter::SqlParameterType::STRING, Strings.size()));
				Strings.push_back(val);
			}

			std::vector<int>& GetInts() {
				return Ints;
			}

			std::vector<double>& GetDoubles() {
				return Doubles;
			}

			std::vector<float>& GetFloats() {
				return Floats;
			}

			std::vector<unsigned int>& GetUnsignedInts() {
				return UnsignedInts;
			}

			std::vector<long>& GetLongs() {
				return Longs;
			}

			std::vector<otl_datetime>& GetOtlDateTimes() {
				return OtlDateTimes;
			}

			std::vector<std::string>& GetStrings() {
				return Strings;
			}

			std::vector<VarInfo>& GetVarInfos() {
				return VarInfos;
			}

			void Clear() {
				VarInfos.clear();
				Ints.clear();
				Doubles.clear();
				Floats.clear();
				UnsignedInts.clear();
				Longs.clear();
				OtlDateTimes.clear();
				Strings.clear();
			}
		};

		class SqlServerSqlWhere
		{
		private:
			std::mutex ConditionMtx;
			std::vector<SqlServerCondition> Conditions;
			bool EnableAutoFillWhere = true;

		public:
			ConditionVars ConditionVars;

		public:

			SqlServerSqlWhere() {
			}

			SqlServerSqlWhere(std::vector<SqlServerCondition> Conditions) {
				this->Conditions = Conditions;
			}

			std::vector<SqlServerCondition> GetConditions() {
				return Conditions;
			}

			/// <summary>
			/// 添加一个条件为‘与’且值为相等的条件
			/// </summary>
			/// <param name="SqlServerCondition">The SqlServerCondition.</param>
			void Add(SqlServerCondition Condition)
			{
				std::unique_lock<std::mutex> lckMtx(ConditionMtx, std::defer_lock);
				lckMtx.lock();
				Conditions.push_back(Condition);
				lckMtx.unlock();
			}

			void Add(SqlServerLogicalOperatorType logicalOperator, SqlServerConditionOperatorType conditionOperator,
				std::string columnName, SqlServerSqlParameter::SqlParameterType parameterType = SqlServerSqlParameter::SqlParameterType::NONE, int parameterTypeSize = -1
			) {
				Conditions.push_back(SqlServerCondition(logicalOperator, conditionOperator, SqlServerSqlParameter(columnName, parameterType, parameterTypeSize)));
			}

			/// <summary>
			/// 添加条件字符串，不安全
			/// </summary>
			/// <param name="sql">The SqlServerCondition SQL.</param>
			/// <param name="logicOperator">The logic operator.</param>
			void AddSql(SqlServerLogicalOperatorType logicOperator, std::string sql)
			{
				if (sql.empty())
				{
					return;
				}
				std::unique_lock<std::mutex> lckMtx(ConditionMtx, std::defer_lock);
				lckMtx.lock();
				Conditions.push_back(SqlServerCondition(logicOperator, sql));
				lckMtx.unlock();
			}

			/// <summary>
			/// 清空现有的条件
			/// </summary>
			void Clear()
			{
				Conditions.clear();
				ConditionVars.Clear();
			}

			std::string GetSql(SqlServerCondition Condition) {

				std::string ConditionSql = "";

				if (Condition.LogicalOperator == SqlServerLogicalOperatorType::AND) {
					ConditionSql.append(" and ");
				}
				else if (Condition.LogicalOperator == SqlServerLogicalOperatorType::OR) {
					ConditionSql.append(" or ");
				}

				if (Condition.GetChild().size() > 0)
				{
					// 分组查询条件处理
					std::string childSql = "(";
					for (auto it = Condition.GetChild().begin(); it != Condition.GetChild().end(); it++) {
						childSql.append(GetSql(*it));
					}

					ConditionSql.append(childSql).append(")");
					return ConditionSql;
				}

				if (!Condition.Sql.empty()) { //直接指定sql的查询条件处理
					ConditionSql.append(Condition.Sql);
				}
				else {
					if (Condition.ConditionOperator == SqlServerConditionOperatorType::IS_NULL) {
						ConditionSql.append(Condition.Parameter.ParameterName).append(" is null");
					}
					else if (Condition.ConditionOperator == SqlServerConditionOperatorType::NOT_NULL) {
						ConditionSql.append(Condition.Parameter.ParameterName).append(" is not null");
					}
					else {
						ConditionSql.append(Condition.Parameter.ParameterName);

						// 默认使用参数的查询条件处理
						if (Condition.ConditionOperator == SqlServerConditionOperatorType::EQ) {
							ConditionSql.append("=:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::NOT_EQ) {
							ConditionSql.append("!=:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::LT) {
							ConditionSql.append("<:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::GT) {
							ConditionSql.append(">:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::LE) {
							ConditionSql.append("<=");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::GE) {
							ConditionSql.append(">=");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::IS_IN) {
							ConditionSql.append(" in (:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::LIKE) {
							ConditionSql.append(" like :");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::NOT_IN) {
							ConditionSql.append(" not in (:");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::NOT_LIKE) {
							ConditionSql.append(" not like :");
						}

						ConditionSql.append(Condition.Parameter.ParameterName);

						switch (Condition.Parameter.ParameterType)
						{
						case SqlServerSqlParameter::SqlParameterType::DOUBLE:
							ConditionSql.append("<double>");
							break;

						case SqlServerSqlParameter::SqlParameterType::FLOAT:
							ConditionSql.append("<float>");
							break;

						case SqlServerSqlParameter::SqlParameterType::INT:
							ConditionSql.append("<int>");
							break;

						case SqlServerSqlParameter::SqlParameterType::UNSIGNED:
							ConditionSql.append("<unsigned int>");
							break;

						case SqlServerSqlParameter::SqlParameterType::LONG:
							ConditionSql.append("<long>");
							break;

						case SqlServerSqlParameter::SqlParameterType::TIMESTAMP:
							ConditionSql.append("<timestamp>");
							break;

						case SqlServerSqlParameter::SqlParameterType::STRING:
							if (Condition.Parameter.ParameterTypeSize >= 0) {
								ConditionSql.append("<char(")
									.append(std::to_string(Condition.Parameter.ParameterTypeSize)).append(")>");
							}
							else {
								ConditionSql.append("<char(1)>");
							}
							break;


						default:
							break;
						}

						if (Condition.ConditionOperator == SqlServerConditionOperatorType::IS_IN) {
							ConditionSql.append(")");
						}
						else if (Condition.ConditionOperator == SqlServerConditionOperatorType::NOT_IN) {
							ConditionSql.append(")");
						}
					}
				}

				return ConditionSql;
			}

			void SetEnableAutoFillWhere(bool isEnable) {
				EnableAutoFillWhere = isEnable;
			}

			std::string GetSql() {
				std::unique_lock<std::mutex> lckMtx(ConditionMtx, std::defer_lock);
				lckMtx.lock();
				std::string sql;

				if (Conditions.size() > 0) {
					if (EnableAutoFillWhere) {
						sql.append(" where ");
					}
				}

				for (auto Condition = Conditions.begin(); Condition != Conditions.end(); Condition++) {
					sql.append(GetSql(*Condition));
				}

				lckMtx.unlock();
				return sql;
			}
		};


































		template<typename T>
		class ObjBase
		{
		protected:
			otl_connect Db;
			std::mutex Mtx;
			bool Connected = false;
			const int FAIL_RETRY_TIME = 2;
			int RetryTime = 0;

			ObjBase() {
			}

			ObjBase(const ObjBase&) {
			}

			ObjBase& operator=(const ObjBase&) {
			}
		protected:
			~ObjBase() {

			}

			typedef void(*ErrorMsgCall)(std::string msg);
			ErrorMsgCall Call = nullptr;

		protected:
			std::string m_odbc_driver;
			std::string m_odbc_server;
			std::string m_odbc_uid;
			std::string m_odbc_pwd;
			std::string m_odbc_database;
			std::string m_odbc_tablename;
		private:
			std::string ConnStr;
		private:
			bool OtlStreamInput(otl_stream& o, ConditionVars& vars) {
				try {
					for (auto it = vars.GetVarInfos().begin(); it != vars.GetVarInfos().end(); it++) {
						if (it->Type == SqlServerSqlParameter::SqlParameterType::INT) {
							o << vars.GetInts().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::DOUBLE) {
							o << vars.GetDoubles().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::FLOAT) {
							o << vars.GetFloats().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::UNSIGNED) {
							o << vars.GetUnsignedInts().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::LONG) {
							o << vars.GetLongs().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::TIMESTAMP) {
							o << vars.GetOtlDateTimes().at(it->Index);
						}
						else if (it->Type == SqlServerSqlParameter::SqlParameterType::STRING) {
							o << vars.GetStrings().at(it->Index);
						}
					}
				}
				catch (otl_exception& p) {
					SetExcpetion(p);
					return false;
				}
				return true;
			}

		public:
			static ObjBase<T>& GetInstance() {
				static ObjBase<T> instance;
				return instance;
			}

			bool Initial(const std::string& odbc_driver, const std::string odbc_server, const std::string& odbc_uid, const std::string& odbc_pwd, const std::string& odbc_database, std::string odbc_tablename = "")
			{
				m_odbc_driver = odbc_driver;
				m_odbc_server = odbc_server;
				m_odbc_uid = odbc_uid;
				m_odbc_pwd = odbc_pwd;
				m_odbc_database = odbc_database;
				m_odbc_tablename = odbc_tablename;

				ConnStr.clear();
				char connstr[MAX_PATH] = { 0 };
				auto nconnstr = sprintf_s(connstr, "driver=%s;server=%s;uid=%s;pwd=%s;database=%s;", m_odbc_driver.c_str(), m_odbc_server.c_str(), m_odbc_uid.c_str(), m_odbc_pwd.c_str(), m_odbc_database.c_str());
				ConnStr.append(connstr, nconnstr);
				return !ConnStr.empty();
			}

			void RegisterErrorMsgCall(ErrorMsgCall call) {
				this->Call = call;
			}

			bool Connect() {
				if (this->ConnStr.empty()) {
					return false;
				}
				if (Connected) return true;

				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				try {
					otl_connect::otl_initialize();
					//!说明 : rlogon(...,0?1)
					//!注意 : aauto_commit设置为1，表示数据库事务自动提交；设置为0，表示数据库事务非自动提交
					//!日期 : Sat Jun 6 12:30:40 UTC+0800 2020
					Db.rlogon(this->ConnStr.c_str(),1);
					if (Db.connected) {
						Connected = true;
					}
				}
				catch (otl_exception& p) {
					SetExcpetion(p);
					return false;
				}
				return true;
			}

			bool AllowConnect() {
				bool connected = Connect();
				DisConnect();
				return connected;
			}

			bool TryConnect() {
				if (!Connected) {
					if (!Connect()) {
						return false;
					}
				}
				return true;
			}

			bool GetModel(T& model) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();

				bool ret = true;

				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("select * from ");
						sql.append(m_odbc_tablename);
						sql.append(" ");
						sql.append(" where id=:id<int>");

						otl_stream o(50, sql.c_str(), Db);

						o << model.id;

						std::vector<T> v;

						copy(otl_input_iterator<T, ptrdiff_t>(o),
							otl_input_iterator<T, ptrdiff_t>(),
							back_inserter(v));

						o.close();

						if (v.size() > 0) {
							model = v.at(0);
						}
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return GetModel(m_odbc_tablename, model);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool GetModelList(std::vector<T>& models) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("select * from ");
						sql.append(m_odbc_tablename);

						otl_stream o(50, sql.c_str(), Db);

						copy(otl_input_iterator<T, ptrdiff_t>(o),
							otl_input_iterator<T, ptrdiff_t>(),
							back_inserter(models));
						o.close();
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return GetModelList(models);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool GetModels(SqlServerSqlWhere& sqlWhere, std::vector<T>& models) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						otl_stream o(50, sqlWhere.GetSql().c_str(), Db);
						OtlStreamInput(o, sqlWhere.ConditionVars);

						copy(otl_input_iterator<T, ptrdiff_t>(o),
							otl_input_iterator<T, ptrdiff_t>(),
							back_inserter(models));

						o.close();
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return GetModels(sqlWhere, models);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool GetModelList(SqlServerSqlWhere& sqlWhere, std::vector<T>& models) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("select * from ");
						sql.append(m_odbc_tablename);
						sql.append(sqlWhere.GetSql());

						otl_stream o(50, sql.c_str(), Db);
						OtlStreamInput(o, sqlWhere.ConditionVars);

						copy(otl_input_iterator<T, ptrdiff_t>(o),
							otl_input_iterator<T, ptrdiff_t>(),
							back_inserter(models));

						o.close();
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return GetModelList(sqlWhere, models);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool GetModelList(const std::string& sqlWhere, std::vector<T>& models) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("select * from ");
						sql.append(m_odbc_tablename);
						sql.append(" ");
						sql.append(sqlWhere);

						otl_stream o(50, sql.c_str(), Db);

						copy(otl_input_iterator<T, ptrdiff_t>(o),
							otl_input_iterator<T, ptrdiff_t>(),
							back_inserter(models));

						o.close();
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return GetModelList(sqlWhere, models);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool Add(std::string sql, T& model) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					otl_stream o(50, sql.c_str(), Db);
					try {
						std::vector<T> vo;
						vo.push_back(model);
						copy(vo.begin(), vo.end(), otl_output_iterator<T>(o));
						o.close();
					}
					catch (otl_exception& p) {
						o.flush();
						o.close();
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return Add(sql, model);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool DeleteSingle(int id) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("delete from ");
						sql.append(m_odbc_tablename);
						sql.append(" where ");
						sql.append("id=:id<int>");
						otl_stream o(50, sql.c_str(), Db);
						o << id;
						o.close();
					}
					catch (otl_exception& p) {
						SetExcpetion(p);
						ret = false;
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return DeleteSingle(m_odbc_tablename, id);
				}
				else {
					RetryTime = 0;
				}
				return true;
			}

			bool Delete(SqlServerSqlWhere& sqlWhere) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("delete from ");
						sql.append(m_odbc_tablename);
						sql.append(sqlWhere.GetSql());
						otl_stream o(50, sql.c_str(), Db);
						OtlStreamInput(o, sqlWhere.ConditionVars);
						o.close();
					}
					catch (otl_exception& p) {
						ret = false;
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return Delete(m_odbc_tablename, sqlWhere);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool UpdateSingle(const std::string sql, T& model) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						otl_stream o(50, sql.c_str(), Db);
						std::vector<T> vo;
						vo.push_back(model);
						copy(vo.begin(), vo.end(), otl_output_iterator<T>(o));
						o << model.id;
						o.close();
					}
					catch (otl_exception& p) {
						SetExcpetion(p);
						ret = false;
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return UpdateSingle(sql, model);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool Update(SqlServerSqlWhere& sqlWhere) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						otl_stream o(50, sqlWhere.GetSql().c_str(), Db);
						OtlStreamInput(o, sqlWhere.ConditionVars);
						o.close();
					}
					catch (otl_exception& p) {
						SetExcpetion(p);
						ret = false;
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return Update(sqlWhere);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			bool Update(SqlServerSqlWhere& sqlWhere, T& model) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				if (ret = TryConnect()) {
					try {
						otl_stream o(50, sqlWhere.GetSql().c_str(), Db);
						std::vector<T> vo;
						vo.push_back(model);
						copy(vo.begin(), vo.end(), otl_output_iterator<T>(o));
						OtlStreamInput(o, sqlWhere.ConditionVars);
						o.close();
					}
					catch (otl_exception& p) {
						SetExcpetion(p);
						ret = false;
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return Update(sqlWhere, model);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			int Count(SqlServerSqlWhere& sqlWhere) {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				int ret = -1;
				if (ret = TryConnect()) {
					try {
						std::string sql;
						sql.append("select COUNT(*) from ");
						sql.append(m_odbc_tablename);
						sql.append(sqlWhere.GetSql());
						otl_stream o(50, sql.c_str(), Db);
						OtlStreamInput(o, sqlWhere.ConditionVars);

						o >> ret;
						o.close();
					}
					catch (otl_exception& p) {
						SetExcpetion(p);
					}
				}
				lckMtx.unlock();
				if (!Connected && (++RetryTime <= FAIL_RETRY_TIME)) {
					return Count(m_odbc_tablename, sqlWhere);
				}
				else {
					RetryTime = 0;
				}
				return ret;
			}

			std::string BoolToString(bool bval) {
				std::stringstream ss;
				ss << bval;
				return ss.str();
			}

			void SetExcpetion(otl_exception& p) {
				std::string sqlstate = (char*)p.sqlstate;

				if (!sqlstate.empty()) {
					if (sqlstate == "01000") {
						Connected = false;
					}
					else if (sqlstate == "01002") {
						Connected = false;
					}
					else if (sqlstate >= "08001" && sqlstate <= "08004") {
						Connected = false;
					}
					else if (sqlstate == "08007") {
						Connected = false;
					}
					else if (sqlstate == "08502") {
						Connected = false;
					}
					else if (sqlstate == "08504") {
						Connected = false;
					}
					else if (sqlstate == "08S01") {
						Connected = false;
					}
					else if (sqlstate == "23000") {
						Connected = false;
					}
				}

				std::string errorMsg;
				errorMsg.append("exception error,sqlsate[");
				errorMsg.append(sqlstate);
				errorMsg.append("]");
				errorMsg.append(",Connected[");
				errorMsg.append(BoolToString(Connected));
				errorMsg.append("]");

				if (!Connected) {
					Db.logoff();
				}

				if (Call != NULL) {
					Call(errorMsg);
				}
#ifdef _DEBUG
				//throw errorMsg;
#else
#endif
			}

			bool DisConnect() {
				std::unique_lock<std::mutex> lckMtx(Mtx, std::defer_lock);
				lckMtx.lock();
				bool ret = true;
				try {
					Db.logoff();
				}
				catch (otl_exception& p) {
					SetExcpetion(p);
					ret = false;
				}
				lckMtx.unlock();
				return ret;
			}
		};





		class Column {
		public:
			std::string Name;
			int DbType = -1;
			size_t DbSize = 0;
			std::string CppType;
			std::string PlaceHolder; // 占位符

			Column() {
			}

			Column(std::string name, int dbType, size_t dbSize, std::string cppType, std::string placeHolder) {
				this->Name = name;
				this->DbType = dbType;
				this->DbSize = dbSize;
				this->CppType = cppType;
				this->PlaceHolder = placeHolder;
			}
		};
		class AutoGenerateTool {
		private:
			const std::string DECLARE_TEMPLATE = "	@type @name;\n";
			const std::string INITIAL_TEMPLATE = "		this->@name = model.@name;\n";
			const std::string EQUAL_TEMPLATE = "		this->@name = model.@name;\n";
			const std::string RIGHT_TEMPLATE = "    s >> model.@name;\n";
			const std::string LEFT_TEMPLATE = "    s << model.@name;\n";
			const std::string IDENTITY_COL = "id";
			const std::string PRIMARY_KEY_COL = "id";

			otl_connect Db;

		public:
			AutoGenerateTool() {
			}

			void Connect(std::string connStr) {
				otl_connect::otl_initialize(); // initialize OCI environment
				Db.rlogon(connStr.c_str());
				Db.auto_commit_off();
			}

			void GetTableNames(std::string connStr, std::vector<std::string>& tableNames) {
				try {
					Connect(connStr);
					otl_stream o(200, "select name from sys.tables", Db);

					std::string tableName;
					while (!o.eof()) {
						o >> tableName;
						tableNames.push_back(tableName);
					}
					o.close();
					Db.logoff();
				}
				catch (otl_exception& p) { // intercept OTL exceptions
					printf("insert Error: (%s) (%s) (%s)\n", p.msg, p.stm_text, p.var_info);
				}
			}

			bool GetColumns(std::string connStr, std::string tableName, std::vector<Column>& columns) {

				Connect(connStr);

				try {
					std::string columnDescSql;
					columnDescSql.append("select top 1 * from ");
					columnDescSql.append(tableName);

					otl_stream o(200, columnDescSql.c_str(), Db);

					otl_column_desc* desc;
					int desc_len;

					desc = o.describe_select(desc_len);

					for (int n = 0; n < desc_len; n++) {
						std::string placeHolder;
						std::string cppType;

						placeHolder.append(":");
						placeHolder.append(desc[n].name);

						switch (desc[n].otl_var_dbtype)
						{
						case otl_var_char:
							placeHolder.append("<");
							placeHolder.append("char");
							placeHolder.append("[");
							placeHolder.append(std::to_string(desc[n].dbsize + 2));
							placeHolder.append("]");
							placeHolder.append(">");
							cppType = "std::string";
							break;

						case otl_var_double:
							placeHolder.append("<double>");
							cppType = "double";
							break;

						case otl_var_float:
							placeHolder.append("<float>");
							cppType = "float";
							break;

						case otl_var_int:
						case otl_var_short:
							placeHolder.append("<int>");
							cppType = "int";
							break;

						case otl_var_unsigned_int:
							placeHolder.append("<unsigned>");
							cppType = "unsigned int";
							break;

						case otl_var_long_int:
							placeHolder.append("<long>");
							cppType = "long";
							break;

						case otl_var_timestamp:
							placeHolder.append("<timestamp>");
							cppType = "otl_datetime";
							break;

						case otl_var_varchar_long:
							placeHolder.append("<varchar_long>");
							cppType = "otl_long_string";
							break;

						case otl_var_raw_long:
							placeHolder.append("<raw_long>");
							cppType = "otl_long_string";
							break;

						case otl_var_bigint:
							placeHolder.append("<bigint>");
							break;

						case otl_var_raw:
							placeHolder.append("<");
							placeHolder.append("raw");
							placeHolder.append("[");
							placeHolder.append(std::to_string(desc[n].dbsize));
							placeHolder.append("]");
							placeHolder.append(">");
							cppType = "std::string";
							break;

						case otl_var_ubigint:
							placeHolder.append("<ubigint>");
							break;
						}
						Column column((std::string)desc[n].name, desc[n].otl_var_dbtype, desc[n].dbsize, cppType, placeHolder);
						columns.push_back(column);
					}
					o.close();
					Db.logoff();
				}
				catch (otl_exception& p) { // intercept OTL exceptions
					printf("insert Error: (%s) (%s) (%s)\n", p.msg, p.stm_text, p.var_info);
					Db.logoff();
					return false;
				}
				return true;
			}

			void GenerateDeclare(std::vector<Column>& columns, std::string& declare) {
				int n = 0;
				std::string declareTemplate;
				for (; n < columns.size(); n++) {
					declareTemplate = DECLARE_TEMPLATE;
					SqlServerUtil::Replace(declareTemplate, "@type", columns[n].CppType);
					SqlServerUtil::Replace(declareTemplate, "@name", columns[n].Name);
					if (n >= columns.size() - 1) {
						SqlServerUtil::Replace(declareTemplate, "\n", "");
					}
					declare.append(declareTemplate);
				}
			}

			void GenerateInitial(std::vector<Column>& columns, std::string& initial) {
				int n = 0;
				std::string initialTemplate;
				for (; n < columns.size(); n++) {
					initialTemplate = INITIAL_TEMPLATE;
					SqlServerUtil::Replace(initialTemplate, "@name", columns[n].Name);
					if (n >= columns.size() - 1) {
						SqlServerUtil::Replace(initialTemplate, "\n", "");
					}
					initial.append(initialTemplate);
				}
			}

			void GenerateEqual(std::vector<Column> columns, std::string& equal) {
				int n = 0;
				std::string equalTemplate;
				for (; n < columns.size(); n++) {
					equalTemplate = EQUAL_TEMPLATE;
					SqlServerUtil::Replace(equalTemplate, "@name", columns[n].Name);
					if (n >= columns.size() - 1) {
						SqlServerUtil::Replace(equalTemplate, "\n", "");
					}
					equal.append(equalTemplate);
				}
			}

			void GenerateRight(std::vector<Column> columns, std::string& right) {
				int n = 0;
				std::string rightTemplate;
				for (; n < columns.size(); n++) {
					rightTemplate = RIGHT_TEMPLATE;
					SqlServerUtil::Replace(rightTemplate, "@name", columns[n].Name);
					if (n >= columns.size() - 1) {
						SqlServerUtil::Replace(rightTemplate, "\n", "");
					}
					right.append(rightTemplate);
				}
			}

			void GenerateLeft(std::vector<Column> columns, std::string& left) {
				size_t n = 0;
				std::string leftTemplate;
				for (; n < columns.size(); n++) {
					leftTemplate = LEFT_TEMPLATE;
					SqlServerUtil::Replace(leftTemplate, "@name", columns[n].Name);
					if (n >= columns.size() - 1) {
						SqlServerUtil::Replace(leftTemplate, "\n", "");
					}
					else {
						if (columns[n + 1].Name == "id") {
							SqlServerUtil::Replace(leftTemplate, "\n", "");
						}
					}

					if (columns[n].Name == "id") {
						continue;
					}

					left.append(leftTemplate);
				}
			}

			void GetUpdateSql(std::vector<Column>& columns, std::string& sql) {
				sql.append("update ");
				sql.append("{}");
				sql.append(" set ");

				Column idColumn;

				int n = 0;
				for (; n < columns.size(); ++n) {
					if (PRIMARY_KEY_COL != columns[n].Name) {
						sql.append(columns[n].Name);
						sql.append("=");

						sql.append(columns[n].PlaceHolder);

						if (n < columns.size() - 1) {
							sql.append(",");
						}
					}
					else {
						if (n >= columns.size() - 1) {
							if ("," == sql.substr(sql.length() - 2, sql.length() - 1)) {
								sql = sql.substr(0, sql.length() - 1);
							}
						}
					}

					if ("id" == columns[n].Name) {
						idColumn = columns[n];
					}
				}

				if (idColumn.Name == "id") {
					sql.append(" where ");
					sql.append(idColumn.Name);
					sql.append("=");
					sql.append(idColumn.PlaceHolder);
				}
			}

			void GetAddSql(std::vector<Column>& columns, std::string& sql) {
				sql.append("insert into ");
				sql.append("{}");
				sql.append(" values(");

				int n = 0;
				for (; n < columns.size(); ++n) {

					if (columns[n].Name != IDENTITY_COL) {
						sql.append(columns[n].PlaceHolder);

						if (n < columns.size() - 1) {
							sql.append(",");
						}
						else {
							sql.append(")");
						}
					}
					else {
						if (n >= columns.size() - 1) {
							if ("," == sql.substr(sql.length() - 2, sql.length() - 1)) {
								sql = sql.substr(0, sql.length() - 1);
							}
							sql.append(")");
						}
					}
				}
			}

			std::string ChangeTableName(std::string& str)
			{
				std::string ret = "";
#if 0
				for each (auto var in SqlServerUtil::Split(str, "_"))
				{
					var[0] = toupper(var[0]);
					ret.append(var);
				}
#else
				for (auto var : SqlServerUtil::Split(str, "_"))
				{
					var[0] = toupper(var[0]);
					ret.append(var);
				}
#endif
				return ret;
			}

			void GenerateModel(std::string templatePath, std::string saveDir, std::string connStr)
			{
				//std::string connStr = SqlServerUtil::ReadFile(connStrPath.c_str());
				std::vector<std::string> tableNames;
				GetTableNames(connStr, tableNames);

				for (int i = 0; i < tableNames.size(); i++) {
					std::vector<Column> columns;

					if (GetColumns(connStr, tableNames.at(i), columns)) {

						std::string declare;
						GenerateDeclare(columns, declare);

						std::string initial;
						GenerateInitial(columns, initial);

						std::string equal;
						GenerateEqual(columns, equal);

						std::string right;
						GenerateRight(columns, right);

						std::string left;
						GenerateLeft(columns, left);

						std::string addSql;
						GetAddSql(columns, addSql);

						std::string updateSql;
						GetUpdateSql(columns, updateSql);

						std::string fileContent = SqlServerUtil::ReadFile(templatePath.c_str());
						std::string entityName = ChangeTableName(tableNames.at(i));

						SqlServerUtil::Replace(fileContent, "@daltblname", tableNames.at(i));
						SqlServerUtil::Replace(fileContent, "@declare", declare);
						SqlServerUtil::Replace(fileContent, "@initial", initial);
						SqlServerUtil::Replace(fileContent, "@equal", equal);
						SqlServerUtil::Replace(fileContent, "@right", right);
						SqlServerUtil::Replace(fileContent, "@left", left);
						SqlServerUtil::Replace(fileContent, "@add", addSql);
						SqlServerUtil::Replace(fileContent, "@tablename", entityName);
						SqlServerUtil::Replace(fileContent, "@update", updateSql);

						std::string newPath;
						newPath.append(saveDir);
						newPath.append(entityName);
						newPath.append(".hpp");

						SqlServerUtil::WriteFile(newPath.c_str(), fileContent);
					}
				}
			}



			//!签名 : GenerateModel
			//!说明 : 
			//!注意 : 
			//!日期 : Sun May 31 23:28:18 UTC+0800 2020
			void GenerateModel(
				__in const std::string& ModelPath,
				__in const std::string& GenerateFilesPath,
				__in const std::string& connStr,
				__in const std::string& DBName
				)
			{
				std::vector<std::string> headfiles;
				std::vector<std::string> tableNames;
				GetTableNames(connStr, tableNames);
				for (int i = 0; i < tableNames.size(); i++) {
					std::vector<Column> columns;
					if (GetColumns(connStr, tableNames.at(i), columns)) {
						std::string declare;
						GenerateDeclare(columns, declare);
						std::string initial;
						GenerateInitial(columns, initial);
						std::string equal;
						GenerateEqual(columns, equal);
						std::string right;
						GenerateRight(columns, right);
						std::string left;
						GenerateLeft(columns, left);
						std::string addSql;
						GetAddSql(columns, addSql);
						std::string updateSql;
						GetUpdateSql(columns, updateSql);
						std::string fileContent = SqlServerUtil::ReadFile(ModelPath.c_str());
						std::string entityName = ChangeTableName(tableNames.at(i));
						SqlServerUtil::Replace(fileContent, "@daltblname", tableNames.at(i));
						SqlServerUtil::Replace(fileContent, "@declare", declare);
						SqlServerUtil::Replace(fileContent, "@initial", initial);
						SqlServerUtil::Replace(fileContent, "@equal", equal);
						SqlServerUtil::Replace(fileContent, "@right", right);
						SqlServerUtil::Replace(fileContent, "@left", left);
						SqlServerUtil::Replace(fileContent, "@add", addSql);
						SqlServerUtil::Replace(fileContent, "@tablename", entityName);
						SqlServerUtil::Replace(fileContent, "@update", updateSql);
						SqlServerUtil::Replace(fileContent, "@dbconnstr", connStr);
						std::string newPath;
						newPath.append(GenerateFilesPath);
						newPath.append("\\");
						newPath.append(DBName);
						newPath.append("\\");
						newPath.append(entityName);
						newPath.append(".hpp");
						SqlServerUtil::WriteFile(newPath.c_str(), fileContent);
						headfiles.emplace_back(entityName + ".hpp");
					}
				}

				if(!headfiles.empty())
				{
					std::string fileContent("#pragma once");
					fileContent.append("\n");
					fileContent.append(R"(#include "../../stdafx.h")");
					fileContent.append("\n");
					for (const auto& include : headfiles)
					{
						fileContent.append(R"(#include ")").append(include).append(R"(")").append("\n");
					}

					std::string newPath;
					newPath.append(GenerateFilesPath);
					newPath.append("\\");
					newPath.append(DBName);
					newPath.append("\\");
					newPath.append(DBName);
					newPath.append(".hpp");
					SqlServerUtil::WriteFile(newPath.c_str(), fileContent);
				}



			}




		};


		enum struct EnDBStatus : UINT
		{
			EN_DBSTATUS_DISCONNECTED = 0,
			EN_DBSTATUS_CONNECTING = 1,
			EN_DBSTATUS_CONNECTED = 2,
			EN_DBSTATUS_SYNCHRONIZED = 4,

			EN_NETSTATUS_FIRST = EN_DBSTATUS_DISCONNECTED,
			EN_NETSTATUS_LAST = EN_DBSTATUS_SYNCHRONIZED,
		};

		class OTLHelper final
		{
		public:
			OTLHelper(
				const std::string& db_driver,
				const std::string& db_server,
				const std::string& db_uid,
				const std::string& db_pwd
			)
			{
				m_db_driver = db_driver;
				m_db_server = db_server;
				m_db_uid = db_uid;
				m_db_pwd = db_pwd;
				Open();
			}

			~OTLHelper()
			{
				Close();
			}
		private:
			int Connect()
			{
				Status(EnDBStatus::EN_DBSTATUS_CONNECTING);
				char szConnect[MAX_PATH] = { 0 };
				sprintf_s(szConnect, "driver=%s;server=%s;uid=%s;pwd=%s;", m_db_driver.c_str(), m_db_server.c_str(), m_db_uid.c_str(), m_db_pwd.c_str());

				try {
					otl_connect::otl_initialize();
					m_db.rlogon(szConnect, 1);
					if (m_db.connected)
					{
						Status(EnDBStatus::EN_DBSTATUS_CONNECTED);
					}
				}
				catch (otl_exception& ExceptionInfo) {
					Exception(ExceptionInfo);
					return -1;
				}
				return 0;
			}
			int TryConnect()
			{
				if (Status() < EnDBStatus::EN_DBSTATUS_CONNECTED)
				{
					return Connect();
				}
				return 0;
			}
			void Exception(otl_exception& Exception)
			{
				std::string sqlstate = (char*)Exception.sqlstate;

				if (!sqlstate.empty()) 
				{
					if (sqlstate == "01000") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "01002") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate >= "08001" && sqlstate <= "08004") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "08007") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "08502") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "08504") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "08S01") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
					else if (sqlstate == "23000") {
						Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);
					}
				}

				if (Status()<EnDBStatus::EN_DBSTATUS_CONNECTING) 
				{
					m_db.logoff();
				}
			}

			int Open()
			{
				if (m_IsOpen.load()) return 0;
				if (Connect()) { return -1; }
				m_IsOpen.store(true);
				return 0;
			}
			int Close()
			{
				if (!m_IsOpen.load()) return 0;				
				m_db.logoff();
				Status(EnDBStatus::EN_DBSTATUS_DISCONNECTED);


				m_IsOpen.store(false);
				return 0;
			}
		public:
			EnDBStatus Status() const { return m_DBStatus.load(); }
			void Status(const EnDBStatus& status) { m_DBStatus.store(status); }

			template<typename T>
			int Exec(const std::string& database, const std::string& table,const std::string& condition,std::vector<T>& out)
			{
				std::lock_guard<std::mutex> lock{ m_mutex };
				if (TryConnect()) { return -1; }
				int result = 0;
				try {
					std::string sql;
					sql.append("select * from ").append(database).append("..").append(table).append(" ");
					sql.append(condition);

					otl_stream o(50, sql.c_str(), m_db);
					std::copy(otl_input_iterator<T, ptrdiff_t>(o),
						otl_input_iterator<T, ptrdiff_t>(),
						std::back_inserter(out));
					o.close();
				}
				catch (otl_exception& ExceptionInfo) {
					result = -1;
					Exception(ExceptionInfo);
				}
				return result;
			}

			template<typename T>
			int Exec(const std::string& sql, std::vector<T>& out)
			{
				std::lock_guard<std::mutex> lock{ m_mutex };
				if (TryConnect()) { return -1; }
				int result = 0;
				try {
					otl_stream o(50, sql.c_str(), m_db);
					std::copy(otl_input_iterator<T, ptrdiff_t>(o),
						otl_input_iterator<T, ptrdiff_t>(),
						std::back_inserter(out));
					o.close();
				}
				catch (otl_exception& ExceptionInfo) {
					result = -1;
					Exception(ExceptionInfo);
				}
				return result;
			}
		private:
			std::string m_db_driver;
			std::string m_db_server;
			std::string m_db_uid;
			std::string m_db_pwd;
			otl_connect m_db;
			std::mutex m_mutex;
			std::atomic_bool m_IsOpen = false;
			std::atomic<EnDBStatus> m_DBStatus = EnDBStatus::EN_DBSTATUS_DISCONNECTED;
		};




	}///namespace otl
}///namespace shared







///!@ 新生联创®（上海）
///!@ Sun May 31 19:07:48 UTC+0800 2020
///!@ ___www.skstu.com___
#endif///AFX_SK_H__72CCE109_9AE7_437C_8E40_CAEFF2CD332C__HEAD__