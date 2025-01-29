/****************************************************************************
MIT License

Copyright (c) 2024 zishun zhou (zhouzishun@mail.zzshub.cn)
Copyright (C) 2021-2022, Qingqing Li (liqingmuguang@163.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*****************************************************************************/
#pragma once

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>


namespace lee
{
    namespace blocks
    {
        template <typename _T = double>
        class LLog
        {
        public:
            using Ptr = std::shared_ptr<LLog<_T>>;
            static Ptr Create()
			{
				return std::make_shared<LLog<_T>>();
			}

        public:
            LLog()
            {
                this->CurrentDataIndex = 0;
            };
            inline void startLog()
            {
                this->CurrentDataIndex = 0;
            };
            void addLog(const _T& _SingleData, const char* _Name = "")
            {
                this->CurrentDataIndex++;
                if ((int)this->DataName.size() < this->CurrentDataIndex)
                {
                    this->DataName.push_back(_Name);
                    if ((int)this->DataList.size() < this->CurrentDataIndex)
                    {
                        this->DataList.push_back({ _SingleData });
                        return;
                    }
                }
                this->DataList[this->CurrentDataIndex - 1].push_back(_SingleData);
            };
            void saveLog(const char* _FileName)
            {
                this->FileStream.open(_FileName, std::fstream::out);
                for (auto i : this->DataName)
                    this->FileStream << i << ",";
                this->FileStream << std::endl;
                for (int i = 0; i < this->DataList[0].size(); i++)
                {
                    for (int j = 0; j < this->DataName.size(); j++)
                    {
                        this->FileStream << std::setprecision(12) << this->DataList[j][i] << ", ";
                    }
                    this->FileStream << std::endl;
                }
                this->FileStream.close();
            };

            inline auto& getDataList() { return this->DataList; };
            inline auto& getNameList() { return this->DataName; };
            inline auto getIndex(const char* _Str)
            {
                auto tar_iter = std::find(this->DataName.begin(), this->DataName.end(), _Str);
                return std::distance(this->DataName.begin(), tar_iter);
            };
            inline auto& getData(const char* _Str)
            {
                return this->getDataList()[this->getIndex(_Str)];
            };

            void initMemory(const unsigned int& _Kinds, const unsigned int& _Num)
            {
                this->DataList.resize(_Kinds);
                for (int i = 0; i < _Kinds; i++)
                {
                    this->DataList[i].reserve(_Num);
                }
            };

            void checkMemory()
            {
                std::cout << "Logger Memory Check:" << std::endl;
                for (int i = 0; i < this->DataList.size(); i++)
                {
                    std::cout << "Data " << i << " Capacity: " << this->DataList[i].capacity() << std::endl;
                }
            };

        protected:
            std::vector<std::vector<_T>> DataList;
            std::vector<std::string> DataName;
            int CurrentDataIndex;
            std::fstream FileStream;
        };
    }
}

namespace zzs
{
    class CSVReader
    {
    public:
        using ReadStageCallback = std::function<void (bool&)>;
    public:
        using Ptr=std::shared_ptr<CSVReader>;
        static Ptr Create(const std::string& _FileName, const bool& _HasHeader = true)
        {
            return std::make_shared<CSVReader>(_FileName, _HasHeader);
        }

    public:
        CSVReader(std::string filename, bool has_header = true)
            : filename_(filename), has_header(has_header),
            ReadStageCallbackPtr__(nullptr)
        {

           
        }

        void RegistReadStateCallback(const ReadStageCallback& func)
        {
            this->ReadStageCallbackPtr__ = func;
        }

        bool open()
        {
            //open file and read data
            std::ifstream file(filename_);
            if (!file.is_open())
            {
                std::cout << "Error opening file" << std::endl;
                return false;
            }
            else
            {
                try
                {
                    int row_count = 0;
                    std::string line;
                    while (std::getline(file, line))
                    {
                        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

                        std::vector<std::string> row;
                        row = stringSplit(line, ',');
                        if (has_header && row_count == 0)
                        {
                            for (int i = 0; i < row.size(); i++)
                            {
                                if (!row[i].empty())
                                    header_[row[i]] = i;
                            }
                        }
                        else
                        {
                            //data_.push_back(row);
                            std::vector<double> row_numeric;
                            for (const auto& item : row)
                            {
                                if (!item.empty())
                                    row_numeric.push_back(std::stod(item));
                            }
                            data_numeric_.push_back(row_numeric);
                            //qDebug() << "reading line:" << data_numeric_.size();
                        }
                        row_count++;
                        bool cancel=false;
                        if(this->ReadStageCallbackPtr__!=nullptr)
                            this->ReadStageCallbackPtr__(cancel);
                        if (cancel)
                        {
                            return false;
                        }
                    }

                    file.close();
                    //qDebug() << "closing file";
                }
                catch (const std::exception& e)
                {
                    qDebug() << e.what();
                    return false;
                }
                


                if (data_numeric_.size() == 0)
                    return false;
                size_t first_size = data_numeric_[0].size();
                for (size_t i = 0; i < data_numeric_.size(); i++)
                {
                    if (data_numeric_[i].size() != first_size)
                        return false;
                }
                
                if (!has_header)
                {
                    if (this->data_numeric_.size() != 0)
                    {
                        size_t col_num = this->data_numeric_[0].size();
                        this->header_.clear();
                        for (size_t i = 0; i < col_num; i++)
                        {
                            std::string idx = std::to_string(i);
                            this->header_.insert(std::make_pair(std::string("column") + idx, i));
                        }
                    }
                }
            }
            return true;
        }

        size_t RowSize() const
        {
            return this->data_numeric_.size();
        }

        size_t ColSize() const
        {
            if (this->data_numeric_.size() == 0) return 0;
            return this->data_numeric_[0].size();
        }

        std::map<std::string, int> getHeader()
        {
            return this->header_;
        }

        std::vector<double> getRow(int Row)
        {
            std::vector<double> row;
            if (Row < data_numeric_.size())
            {
                row = data_numeric_[Row];
            }
            return row;
        }

        std::vector<double> getColumn(int Column)
        {
            if (this->data_numeric_.size() == 0)
                return std::vector<double>();

            std::vector<double> column;
            if (Column < data_numeric_[0].size())
            {
                for (int i = 0; i < data_numeric_.size(); i++)
                {
                    column.push_back(data_numeric_[i][Column]);
                }
            }
            return column;
        }

        double getItem(int row, int column)
        {
            if (this->data_numeric_.size() == 0) return 0;

            if (row < data_numeric_.size() && column < data_numeric_[0].size())
            {
                return data_numeric_[row][column];
            }
            else
            {
                std::cout << "Row " << row << " or Column " << column << " not found in data" << std::endl;
                return 0;
            }
        }

        std::vector<double> getItems(const std::vector<std::string>& items, size_t row)
        {
            if (this->data_numeric_.size() == 0)
                return std::vector<double>();

            std::vector<double> result;
            //check row 
            if (row >= data_numeric_.size())
            {
                std::cout << "Row " << row << " not found in data" << std::endl;
                return result;
            }
            for (const auto& item : items)
            {
                if (header_.find(item) != header_.end())
                {
                    result.push_back(data_numeric_[row][header_[item]]);
                }
                else
                {
                    std::cout << "Item " << item << " not found in header" << std::endl;
                }
            }
            return result;
        }

        friend std::ostream& operator<<(std::ostream& os, const CSVReader& reader)
        {
            for (const auto& row : reader.data_numeric_)
            {
                for (const auto& item : row)
                {
                    //print item
                    os << item << " ";
                }
                os << std::endl;
            }
            return os;
        }


    private:
        std::vector<std::string> stringSplit(const std::string& str, char delim) {
            std::size_t previous = 0;
            std::size_t current = str.find(delim);
            std::vector<std::string> elems;
            while (current != std::string::npos) {
                if (current > previous) {
                    elems.push_back(str.substr(previous, current - previous));
                }
                previous = current + 1;
                current = str.find(delim, previous);
            }
            if (previous != str.size()) {
                elems.push_back(str.substr(previous));
            }
            return elems;
        }

    private:
        std::string filename_;
        std::map<std::string, int> header_;
        std::vector<std::vector<double>> data_numeric_;
        bool has_header;
        ReadStageCallback ReadStageCallbackPtr__;
    };
};
