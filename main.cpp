#include <iostream>
#include <cstring>
#include <fstream>

struct Arguments{
  unsigned long long argument_length;
  bool is_there_option = false;
  bool online = false;
  char* argument_with_length = new char[argument_length];
  char* argument;
  bool got_argument = false;
  bool long_arg = false;
  bool short_arg = false;
}; //структура параметры опций
struct Options{
  Arguments lines;
  Arguments delimiter;
  Arguments tail;
  Arguments filename;
}; //структура опции
struct CorrectNames{
  char delimiter[10] = {'d','e','l','i','m','i','t','e','r','='};
  char lines[6] = {'l','i','n','e','s','='};
  char tail[5] = {'t','a','i','l'};
}; //структура с правильным написанием названий опций
struct ResultArgument{
  char delimiter = '\n';
  unsigned long long lines;
  bool tail;
};//структура с финальными аргументами опций с правильными типами данных
struct CheckSlash{
  bool slash = false;
};

CorrectNames correct_names;
Options options;
ResultArgument res_arg;
CheckSlash check_slash;

//РАБОТА С АРГУМЕНТАМИ КОМАНДНОЙ СТРОКИ

//ПОДСОБНЫЕ ФУНКЦИИ

void HelpList() {
  std::cout << std::endl;
  std::cout << "-d '/x' / --delimiter=x ----- delimiter option" << std::endl;
  std::cout << "-l 'number' / --lines='number' ----- lines option" << std::endl;
  std::cout << "-t / --tail ----- tail option";
  std::cout << std::endl;
  std::exit(1);
} //инструкция по работе с приложением
void LastErrorCheck() {
  if (!options.filename.is_there_option) {
    std::cerr << "No file name" << std::endl;
    HelpList();
  }
  if (!options.lines.is_there_option && options.tail.is_there_option) {
    std::cerr << "No tail option argument" << std::endl;
    HelpList();
  }
} //проверка на наличие назвния файл и аргумента для опции tail
void ArgumentErrorCheck(int i, int argc) {
  if (options.delimiter.is_there_option) {
    if ((!options.delimiter.got_argument && !options.delimiter.online && !options.delimiter.long_arg) || (i == argc-1 && !options.delimiter.got_argument && options.delimiter.online)) {
      std::cerr << "Delimetr argument error" << std::endl;
      HelpList();
    }
  }
  if (options.lines.is_there_option) {
    if ((!options.lines.got_argument && !options.lines.online && !options.lines.long_arg)  || (i == argc-1 && !options.lines.got_argument && options.lines.online)) {
      std::cerr << "Lines argument error" << std::endl;
      HelpList();
    }
  }
}//проверка на отсутствие аргумента у коротких опций
bool SpellingCheck(char* argument) {
  if (argument[2] == 'd') {
    for(int i = 1; i < 10; i++){
      if (argument[i+2] != correct_names.delimiter[i]) {
        return false;
      }
    }
    return true;
  }
  else if (argument[2] == 'l') {
    for(int i = 1; i < 6; i++) {
      if(argument[i+2] != correct_names.lines[i]){
        return false;
      }
    }
    return true;
  }
  else if (argument[2] == 't') {
    for(int i = 1; i < 5; i++) {
      if (argument[i+2] != correct_names.tail[i]) {
        return false;
      }
    }
    return true;
  }
  else {
    return false;
  }
}//проверка на корректность написания названия опции
bool IsDigitCheck(char symbol){
  char digits[10] = {'0','1','2','3','4','5','6','7','8','9'};
  for(char digit : digits) {
    if(symbol == digit) {
      return true;
    }
  }
  return false;
}//проверка аргумента опции lines - нужно число
void LongNameLinesInput(char* argument,unsigned long long argument_length) {
  options.lines.argument_length = argument_length - 8;
  if (options.lines.argument_length == 0) {
    std::cout << "Error: No lines argument" <<std::endl;
    HelpList();
  }
  int j = 0;
  for(int i = 8; i < argument_length; i++) {
    if (IsDigitCheck(argument[i])) {
      options.lines.argument_with_length[j++] = argument[i];
    }
    else {
      std::cout << "Error: Wrong lines argument" << std::endl;
      HelpList();
    }
  }
  options.lines.is_there_option = true;
  options.lines.long_arg = true;
} //ввод длинной опции lines и ее аргумента
void LongNameDelimiterInput(char* argument, unsigned long long argument_length) {
  options.delimiter.argument_length = argument_length - 12;
  if (options.delimiter.argument_length == 0 or options.delimiter.argument_length >1) {
    std::cout << "Error: Wrong delimiter argument" <<std::endl;
    HelpList();
  }
  int j = 0;
  for(int i = 11; i < argument_length; i++) {
    options.delimiter.argument_with_length[j] = argument[i];
  }
  options.delimiter.is_there_option = true;
  options.delimiter.long_arg = true;
} //ввод длинной опции delimiter и ее аргумента

//БАЗОВЫЕ ФУНКЦИИ

void Result() {
  if (options.lines.is_there_option) {
    if (options.lines.short_arg) {
      res_arg.lines = std::stoi(options.lines.argument);
    }
    else {
      res_arg.lines = std::stoi(options.lines.argument_with_length);
    }
  }

  if (options.delimiter.is_there_option){
    if (options.delimiter.long_arg){
      res_arg.delimiter = options.delimiter.argument_with_length[0];
    }
    else {
      if (options.delimiter.argument[1] == '\\'){
        check_slash.slash = true;
        res_arg.delimiter = options.delimiter.argument[2];
      }
      else{
        res_arg.delimiter = options.delimiter.argument[1];
      }
    }
  }
  else {
    res_arg.delimiter = '\n';
  }

  res_arg.tail = options.tail.is_there_option;
} //функция преобразует полученные типы данных аргументов в правильные
bool CheckIfDotInArgument(char* argument, unsigned long long argument_length) {
  for(int i = 0; i < argument_length; i++) {
    if(argument[i] == '.') {
      return true;
    }
  }
  return false;
}//проверка есть точка в названии файла или нет
void LongNameOptionInput(char* argument, unsigned long long argument_length) {
  if (SpellingCheck(argument)) {
    if (argument[2] == 'd') {
      if (!options.delimiter.is_there_option){
        //опция delimiter получена
        LongNameDelimiterInput(argument,argument_length);
      }
      else {
        std::cout << "Error: delimeter was already initialized" << std::endl;
        HelpList();
      }
    }
    else if (argument[2] == 'l') {
      if (!options.lines.is_there_option) {
        //опция lines получена
        LongNameLinesInput(argument,argument_length);
      }
      else {
        std::cout << "Error: lines was already initialized" << std::endl;
        HelpList();
      }
    }
    else if (argument[2] == 't') {
      if (!options.tail.is_there_option) {
        //опция tail получена
        options.tail.is_there_option = true;
      }
      else {
        std::cout << "Error: tail was already initialized" << std::endl;
        HelpList();
      }
    }
  }
  else {
    std::cout << "Incorrect long option name" << std::endl;
    HelpList();
  }

}//ввод длинных опций
void ShortNameOptionInput(char* argument) {
  if (argument[1] == 'd') {
    options.delimiter.online = true;
    options.delimiter.is_there_option = true;
  }
  else if (argument[1] == 'l') {
    options.lines.online = true;
    options.lines.is_there_option = true;
  }
  else if (argument[1] == 't') {
    options.tail.is_there_option = true;
  }
  else {
    std::cout << "Incorrect short option name" << std::endl;
    HelpList();
  }
}//функция сообщает следующему проходу цикла что получена опция и нужно считать ее аргумент
void ShortOptionArgumentInput(char* argument, unsigned long long argument_length) {
  if (options.delimiter.online){
    options.delimiter.argument = argument;
    options.delimiter.got_argument = true;
    options.delimiter.online = false;
    options.delimiter.short_arg = true;
  }
  else if (options.lines.online){
    options.lines.argument = argument;
    options.lines.got_argument = true;
    options.lines.online = false;
    options.lines.short_arg = true;
  }
  else {
    std::cerr << "Something wrong with option or argument" << std::endl;
    HelpList();
  }
}//получено сообщение из прошлой функции, считываем аргумент функции
//РАБОТА С ФАЙЛОМ

//ПОДСОБНЫЕ ФУНКЦИИ


//БАЗОВЫЕ ФУНКЦИИ

void FileFirstLinesOutput(int* lines_number, int length, char* online) {
  for(int i = 0; i < length; i++) {
    char symbol = online[i];
    if (options.lines.is_there_option) {
      if ((*lines_number) == res_arg.lines) {
        break;
      }
    }
    if (check_slash.slash) {
      if (i+1 < length) {
        if (symbol == ('\\') and online[i+1] == res_arg.delimiter) {
          std::cout << symbol << online[i+1];
          (*lines_number) ++;
        }
        else {
          std::cout << symbol;
        }
      }

    }
    else {
      if(symbol == res_arg.delimiter) {
        std::cout << symbol;
        (*lines_number) ++;
      }
      else {
        std::cout << symbol;
      }
    }

  }
}//вывод если нет опции tail
void FileLastLinesOutput(int* lines_number,int length) {
  std::ifstream fil (options.filename.argument, std::ifstream::binary);
  char* online = new char[1024];
  fil.read(online,length);
  int count = 0;
  for(int i = 0; i < length; i++) {
    char symbol = online[i];
    if (count > ( (*lines_number) - res_arg.lines )){
      std::cout << symbol;
    }
    if(symbol == res_arg.delimiter) {
      count++;
    }
  }
}//выывод если есть опция tail
void FileNumberOfLines(int* lines_number, int length, char* online) {
  for(int i = 0; i < length; i++) {
    char symbol = online[i];
    if (check_slash.slash) {
      if (i+1 < length) {
        if (symbol == ('\\') and online[i+1] == res_arg.delimiter) {
          (*lines_number) ++;
        }
      }
    }
    else {
      if(symbol == res_arg.delimiter) {
        (*lines_number) ++;
      }
    }
  }
}//подсчет количества переносов

//int FromChar(char* argument){
//  int ten = 1;
//  int k = strlen(argument);
//  int number = 0;
//  for(int i = k-1; i >= 0; i--){
//    number += (argument[i]) * ten;
//    ten *= 10;
//  }
//}

int main(int argc, char* argv[]) {
  unsigned long long argument_length; //длина аргумента argv[i]
  if (argc < 2 or argc > 7) {
    std::cout << "Number of options error" << std::endl;
    HelpList();
  }
  else {
    //цикл проход по элементам argv[i]
    for(int i = 1; i < argc; i++) {
      argument_length = std::strlen(argv[i]);
      char* argument;//массив чаров куда записываем argv[i]
      argument = argv[i];
      if (argument[0] != '-' && CheckIfDotInArgument(argument,argument_length)) {
        options.filename.argument = argument;
        options.filename.is_there_option = true;
      }
      else if (argument[0] != '-' && !CheckIfDotInArgument(argument,argument_length)){
        ShortOptionArgumentInput(argument,argument_length);
      }
      else {
        if (argument[1] == '-') {
          LongNameOptionInput(argument,argument_length);
        }
        else {
          ShortNameOptionInput(argument);
        }
      }
      ArgumentErrorCheck(i,argc);
    }
  }
  LastErrorCheck();
  Result();
  //работа с файлом


  int all_chars = 0;

  int all_delimetr_symbols = 0;


  std::ifstream lab_file;
  lab_file.open(options.filename.argument);

  if (!lab_file.is_open()) {
    std::cout << "FILE NOT OPENED 1";
    return 40;
  } else {

    char char_from_file;
    while (!lab_file.eof()) {

      lab_file.get(char_from_file);
      all_chars++;
      if (char_from_file == res_arg.delimiter) {
        all_delimetr_symbols++;
      }
    }
    lab_file.close();

  }
  std::ifstream lab_file1;
  lab_file1.open(options.filename.argument);

  char char1_from_file1;
  int delimetr_symbol_counter = 0;
  int flag = 0;

  if (!lab_file1.is_open()) {
    std::cout << "FILE NOT OPENED 2";
    return 40;
  } else {
    while (!lab_file1.eof()) {

      char1_from_file1 = lab_file1.get();

      if (!res_arg.tail) {

        //not tail
        if (res_arg.lines == 0) {

          std::cout << char1_from_file1;
        } else if (char1_from_file1 == res_arg.delimiter) {
          delimetr_symbol_counter++;
          std::cout << char1_from_file1;
        } else if (delimetr_symbol_counter < res_arg.lines) {
          std::cout << char1_from_file1;

        } else {
          return 0;
        }

      } else {


        //tail

        if (char1_from_file1 == res_arg.delimiter) {
          delimetr_symbol_counter++;
        }
        if (delimetr_symbol_counter > (all_delimetr_symbols - res_arg.lines)) {
          if (flag == 0) {
            std::cout << '\\';
            flag++;
          }
          std::cout << char1_from_file1;
        }
      }
    }
    lab_file1.close();
  }

}