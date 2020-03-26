

int main()
{
  int dollars[] = {10, 20, 30, 40, 50};
  auto exchange = 1.17;
    auto euro = 0;
    for(auto const& d : dollars){
      euro += d/exchange;
    }
}
