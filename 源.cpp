#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <io.h>
#include <iostream>
#include <windows.h>  
#include <commdlg.h> 
using namespace std;

int wordCount(ifstream& file)//ͳ�Ƶ�����Ŀ��Ӣ�ĵ��ʣ�
{
	file.clear();//�ڵ���seekg֮ǰ������ʹ��clear�����־�����ܶ�λ��ָ����λ��
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

int lineCount(ifstream& file)//ͳ������
{
	file.clear();//�ڵ���seekg֮ǰ������ʹ��clear�����־�����ܶ�λ��ָ����λ��
	file.seekg(ios::beg);
	string str;
	int lineNum = 0;
	while (getline(file, str))
	{
		lineNum++;
	}
	return lineNum;
}

int charCount(ifstream& file)//ͳ���ַ���
{
	file.clear();//�ڵ���seekg֮ǰ������ʹ��clear�����־�����ܶ�λ��ָ����λ��
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
	for (; patternIndex < patternSize; patternIndex++)//�ж�ģ�������Ƿ��йؼ���
	{
		if (pattern[patternIndex] == '*')//����ùؼ�����*
		{
			if (patternIndex != (patternSize - 1) && pattern[patternIndex + 1] == '?')//���*�����ǣ��������Ʋ�WindowsӦ���ǰ���һ���ַ����
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
					if (loc == string::npos)//���*���治�ǣ����Ӻ���ҵ�һ�γ���*��ؼ��ֵ�λ��
					{
						return false;//�Ҳ������ش���
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
	vector<int> sum = { 0,0,0 };//������ ע���� �հ���
	string str;
	int blankLine;//�հ���
	int noteLine;//ע����
	int codeLine;//������
	while (getline(file, str))
	{
		blankLine = 1;//�ٶ������ǿհ���
		noteLine = 0;
		codeLine = 0;
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] != 32)//����Ƿ�Ϊ�ո�
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
		return;//���ļ�ʧ��
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
	return;//�������
}

string extendX()
{
	string path = "";
	OPENFILENAME ofn;      //��ʼ���򿪻򱣴�Ի���    
	TCHAR szFile[MAX_PATH]; // �����ȡ�ļ����ƵĻ�����               
	// ��ʼ��ѡ���ļ��Ի���     
	ZeroMemory(&ofn, sizeof(OPENFILENAME));//�ڴ�λ������
	ofn.lStructSize = sizeof(OPENFILENAME);//ָ������ṹ��Ĵ�С
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;//ָ�������ʼ���ļ����༭�ؼ�ʹ�õ��ļ����Ļ���
	ofn.lpstrFile[0] = '\0';//�������Ҫ��ʼֵ���������ĵ�һ���ַ�������NULL
	ofn.nMaxFile = sizeof(szFile);//ָ��lpstrFile����Ĵ�С����TCHARsΪ��λ
	ofn.lpstrFilter = (LPCSTR)"All(*.*)\0*.*\0Text(*.txt)\0*.TXT\0\0";//ָ��һ���Կ��ַ������Ĺ����ַ�����һ�����壨�����½ǿɹ�ɸѡ���ļ����ͣ������е����һ���ַ�������������NULL�ַ�������
	ofn.nFilterIndex = 1;//ָ����һ�����������ͣ���ALL(*.*)��
	ofn.lpstrFileTitle = NULL;//ָ����ѡ�ļ����ļ�������չ��
	ofn.nMaxFileTitle = 0;//ָ��lpstrFileTitle��С
	ofn.lpstrInitialDir = (LPCSTR)"C:\\Users\\89787\\Desktop";//��ʼ·��
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		path = szFile;
		//std::cout << szFile << std::endl;//��ʾ�ļ�
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
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_cc = true;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{
			if (_wc)
			{
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_wc = true;
		}
		else if (strcmp(argv[i], "-l") == 0)
		{

			if (_lc)
			{
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_lc = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			if (_es)
			{
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_es = true;
		}
		else if (strcmp(argv[i], "-a") == 0)
		{
			if (_ea)
			{
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_ea = true;
		}
		else if (strcmp(argv[i], "-x") == 0)
		{
			if (_ex)
			{
				cout << "���ظ�����������" << endl;
				return 0;
			}
			_ex = true;
		}
		else
		{

		}
	}
	if (_ex && _es)//������ͬʱʹ��-s��-x
	{
		cout << "�������󣺲���ͬʱʹ�ò��� -s �� -x" << endl;
		return 0;
	}
	else if (_ex)//ͼ�ν���
	{
	    string path = extendX();//��ȡ�û���ѡ���ļ�·��
		ifstream file;
		file.open(path);
		if (!file.is_open())//���ļ�ʧ��
		{
			cout << "�޷����ļ�" << endl;
			return 0;
		}
		cout << path << endl;
		if (_wc)//�������ָ��(wc)
		{
			cout << "������ĿΪ��" << wordCount(file) << endl;
		}
		if (_lc)
		{
			cout << "����Ϊ��" << lineCount(file) << endl;
		}
		if (_cc)
		{
			cout << "�ַ���Ϊ��" << charCount(file) << endl;
		}
		if (_ea)
		{
			vector<int> sum = extendA(file);
			cout << "����������" << sum[0] << endl;
			cout << "ע��������" << sum[1] << endl;
			cout << "�հ�������" << sum[2] << endl;
		}
		file.close();
		return 0;
	}
	else if (_es)
	{
		string path = argv[argc - 1];//·��
		string type = argv[argc - 2];//ģ�������ؼ���
		vector<string> pathList;
		extendS(path, pathList, type);
		if (pathList.empty())
		{
			cout << "û���ҵ������������ļ�" << endl;
			return 0;
		}
        for (vector<string>::iterator pos = pathList.begin(); pos != pathList.end(); pos++)
		{
			cout << *pos << endl;//����ļ���
			ifstream file;
			file.open(*pos);
			if (!file.is_open())//����ļ��Ƿ��
			{
				cout << "�޷����ļ�" << endl;
				continue;
			}
			if (_wc)
			{
				cout << "������ĿΪ��" << wordCount(file) << endl;
			}
			if (_lc)
			{
				cout << "����Ϊ��" << lineCount(file) << endl;
			}
			if (_cc)
			{
				cout << "�ַ���Ϊ��" << charCount(file) << endl;
			}
			if (_ea)
			{
				vector<int> sum = extendA(file);
				cout << "����������" << sum[0] << endl;
				cout << "ע��������" << sum[1] << endl;
				cout << "�հ�������" << sum[2] << endl;
			}
			file.close();//�ر��ļ�
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
			cout << "�޷����ļ�,�����ļ�·��" << endl;
			return 0;
		}
		else
		{
			if (_wc)
			{
				cout << "������ĿΪ��" << wordCount(file) << endl;
			}
			if (_lc)
			{
				cout << "����Ϊ��" << lineCount(file) << endl;
			}
			if (_cc)
			{
				cout << "�ַ���Ϊ��" << charCount(file) << endl;
			}
			if (_ea)
			{
				vector<int> sum = extendA(file);
				cout << "����������" << sum[0] << endl;
				cout << "ע��������" << sum[1] << endl;
				cout << "�հ�������" << sum[2] << endl;
			}
		}
		file.close();
		return 0;
	}

	return 0;
}
