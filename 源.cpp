#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <io.h>
#include <iostream>
#include <windows.h>  
#include <commdlg.h> 
using namespace std;

int wordCount(ifstream& file)//统计单词数目（英文单词）
{
	file.clear();//在调用seekg之前必须先使用clear清除标志，才能定位到指定的位置
	file.seekg(ios::beg);
	string str;
	int number = 0;
	while (getline(file, str))
	{
		for (int i = 0; i < str.size(); i++)
		{
			if ((65 <= str[i] && str[i] <= 90) || (97 <= str[i] && str[i] <= 122))
			{
				number++;
				i++;
				while ((65 <= str[i] && str[i] <= 90) || (97 <= str[i] && str[i] <= 122))
				{
					i++;
				}
			}
		}
	}
	return number;
}

int lineCount(ifstream& file)//统计行数
{
	file.clear();//在调用seekg之前必须先使用clear清除标志，才能定位到指定的位置
	file.seekg(ios::beg);
	string str;
	int lineNum = 0;
	while (getline(file, str))
	{
		lineNum++;
	}
	return lineNum;
}

int charCount(ifstream& file)//统计字符数
{
	file.clear();//在调用seekg之前必须先使用clear清除标志，才能定位到指定的位置
	file.seekg(ios::beg);
	string str;
	int charNum = 0;
	while (getline(file, str))
	{
		int i = 0, j = 0;
		while (str[i])
		{
			if ((str[i] & 0xc0) != 0x80)
			{
				j++;
			}
			i++;
		}
		charNum += j;
	}
	return charNum;
}

bool fuzzySearch(string pattern, string str)
{
	int patternSize = pattern.size();
	int strSize = str.size();
	int strIndex = 0;
	int patternIndex = 0;
	for (; patternIndex < patternSize; patternIndex++)//判断模糊搜索是否还有关键字
	{
		if (pattern[patternIndex] == '*')//如果该关键字是*
		{
			if (patternIndex != (patternSize - 1) && pattern[patternIndex + 1] == '?')//如果*后面是？，根据推测Windows应该是按照一个字符算的
			{
				strIndex++;
				patternIndex++;
				continue;
			}
			else
			{
				if (patternIndex == patternSize - 1)
				{
					return true;
				}
				else
				{
					size_t loc = str.rfind(pattern[patternIndex + 1]);
					if (loc == string::npos)//如果*后面不是？，从后查找第一次出现*后关键字的位置
					{
						return false;//找不到返回错误
					}
					else
					{
						strIndex = loc;
					}
				}
			}
		}
		else if (pattern[patternIndex] == '?')
		{
			if (strIndex == strSize - 1)
			{
				return false;
			}
			else
			{
				strIndex++;
			}
		}
		else
		{
			if (pattern[patternIndex] == str[strIndex])
			{
				strIndex++;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

vector<int> extendA(ifstream& file)
{
	file.clear();
	file.seekg(ios::beg);
	vector<int> sum = { 0,0,0 };//代码行 注释行 空白行
	string str;
	int blankLine;//空白行
	int noteLine;//注释行
	int codeLine;//代码行
	while (getline(file, str))
	{
		blankLine = 1;//假定本行是空白行
		noteLine = 0;
		codeLine = 0;
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] != 32)//检查是否为空格
			{
				blankLine = 0;
			}
		}
		if (str.size() <= 1)
		{
			blankLine = 1;
		}
		else
		{
			if ((str[0] == 47 && str[1] == 47) || (str[1] == 47 && str[2] == 47))
			{
				noteLine = 1;
			}
		}
		if (noteLine == 1)
		{
			sum[1]++;
		}
		else if (blankLine == 1)
		{
			sum[2]++;
		}
		else
		{
			sum[0]++;
		}
	}
	return sum;
}

void extendS(string path, vector<string>& pathList, string type)
{
	_finddata_t file_info;
	string mode = "\\*.*";
	string current_path = path + mode;
	long handle = _findfirst(current_path.c_str(), &file_info);
	if (-1L == handle)
	{
		cout << "open file error" << endl;
		return;//打开文件失败
	}
	do
	{
		if (file_info.attrib == _A_SUBDIR)
		{
			if ((strcmp(file_info.name, ".") != 0) && (strcmp(file_info.name, "..") != 0))
			{
				string newPath = path + "\\" + file_info.name;
				extendS(newPath, pathList,type);
			}
		}
		else
		{
			string name = file_info.name;
			if (fuzzySearch(type, name))
			{
				pathList.push_back(path + "\\" + name);
			}
		}
	} while (_findnext(handle, &file_info) == 0);
	_findclose(handle);
	return;//遍历完成
}

string extendX()
{
	string path = "";
	OPENFILENAME ofn;      //初始化打开或保存对话框    
	TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区               
	// 初始化选择文件对话框。     
	ZeroMemory(&ofn, sizeof(OPENFILENAME));//内存位置清零
	ofn.lStructSize = sizeof(OPENFILENAME);//指定这个结构体的大小
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;//指向包含初始化文件名编辑控件使用的文件名的缓冲
	ofn.lpstrFile[0] = '\0';//如果不需要初始值，这个缓冲的第一个字符必须是NULL
	ofn.nMaxFile = sizeof(szFile);//指定lpstrFile缓冲的大小，以TCHARs为单位
	ofn.lpstrFilter = (LPCSTR)"All(*.*)\0*.*\0Text(*.txt)\0*.TXT\0\0";//指向一对以空字符结束的过滤字符串的一个缓冲（即右下角可供筛选的文件类型）缓冲中的最后一个字符串必须以两个NULL字符结束。
	ofn.nFilterIndex = 1;//指定第一个过滤器类型（即ALL(*.*)）
	ofn.lpstrFileTitle = NULL;//指向所选文件的文件名和扩展名
	ofn.nMaxFileTitle = 0;//指定lpstrFileTitle大小
	ofn.lpstrInitialDir = (LPCSTR)"C:\\Users\\89787\\Desktop";//初始路径
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		path = szFile;
		//std::cout << szFile << std::endl;//显示文件
	}
	return path;
}

int main(int argc, char* argv[])
{
	bool _wc = false;
	bool _lc = false;
	bool _cc = false;
	bool _ea = false;
	bool _es = false;
	bool _ex = false;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-c") == 0)
		{
			if (_cc)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_cc = true;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{
			if (_wc)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_wc = true;
		}
		else if (strcmp(argv[i], "-l") == 0)
		{

			if (_lc)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_lc = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			if (_es)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_es = true;
		}
		else if (strcmp(argv[i], "-a") == 0)
		{
			if (_ea)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_ea = true;
		}
		else if (strcmp(argv[i], "-x") == 0)
		{
			if (_ex)
			{
				cout << "有重复参数，请检查" << endl;
				return 0;
			}
			_ex = true;
		}
		else
		{

		}
	}
	if (_ex && _es)//不允许同时使用-s和-x
	{
		cout << "参数错误：不能同时使用参数 -s 和 -x" << endl;
		return 0;
	}
	else if (_ex)//图形界面
	{
	    string path = extendX();//获取用户所选的文件路径
		ifstream file;
		file.open(path);
		if (!file.is_open())//打开文件失败
		{
			cout << "无法打开文件" << endl;
			return 0;
		}
		cout << path << endl;
		if (_wc)//检查其他指令(wc)
		{
			cout << "单词数目为：" << wordCount(file) << endl;
		}
		if (_lc)
		{
			cout << "行数为：" << lineCount(file) << endl;
		}
		if (_cc)
		{
			cout << "字符数为：" << charCount(file) << endl;
		}
		if (_ea)
		{
			vector<int> sum = extendA(file);
			cout << "代码行数：" << sum[0] << endl;
			cout << "注释行数：" << sum[1] << endl;
			cout << "空白行数：" << sum[2] << endl;
		}
		file.close();
		return 0;
	}
	else if (_es)
	{
		string path = argv[argc - 1];//路径
		string type = argv[argc - 2];//模糊搜索关键字
		vector<string> pathList;
		extendS(path, pathList, type);
		if (pathList.empty())
		{
			cout << "没有找到符合条件的文件" << endl;
			return 0;
		}
        for (vector<string>::iterator pos = pathList.begin(); pos != pathList.end(); pos++)
		{
			cout << *pos << endl;//输出文件名
			ifstream file;
			file.open(*pos);
			if (!file.is_open())//检查文件是否打开
			{
				cout << "无法打开文件" << endl;
				continue;
			}
			if (_wc)
			{
				cout << "单词数目为：" << wordCount(file) << endl;
			}
			if (_lc)
			{
				cout << "行数为：" << lineCount(file) << endl;
			}
			if (_cc)
			{
				cout << "字符数为：" << charCount(file) << endl;
			}
			if (_ea)
			{
				vector<int> sum = extendA(file);
				cout << "代码行数：" << sum[0] << endl;
				cout << "注释行数：" << sum[1] << endl;
				cout << "空白行数：" << sum[2] << endl;
			}
			file.close();//关闭文件
		}
			return 0;
	}
	else
	{
		string path = argv[argc - 1];
		ifstream file;
		file.open(path);
		if (!file.is_open())
		{
			cout << "无法打开文件,请检查文件路径" << endl;
			return 0;
		}
		else
		{
			if (_wc)
			{
				cout << "单词数目为：" << wordCount(file) << endl;
			}
			if (_lc)
			{
				cout << "行数为：" << lineCount(file) << endl;
			}
			if (_cc)
			{
				cout << "字符数为：" << charCount(file) << endl;
			}
			if (_ea)
			{
				vector<int> sum = extendA(file);
				cout << "代码行数：" << sum[0] << endl;
				cout << "注释行数：" << sum[1] << endl;
				cout << "空白行数：" << sum[2] << endl;
			}
		}
		file.close();
		return 0;
	}

	return 0;
}
