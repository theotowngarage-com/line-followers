class MyRenderer : public MenuComponentRenderer {
public:
  void render(Menu const& menu) const {
      Serial.print(F("\nCurrent menu name: "));
      Serial.println(menu.get_name());
      String buffer;
      for (int i = 0; i < menu.get_num_components(); ++i) {
          MenuComponent const* cp_m_comp = menu.get_menu_component(i);
          cp_m_comp->render(*this);
  
          if (cp_m_comp->is_current())
              Serial.print(F("<<< "));
          Serial.println();
      }
  }
  
  void render_menu_item(MenuItem const& menu_item) const {
      Serial.print(menu_item.get_name());
  }
  
  void render_back_menu_item(BackMenuItem const& menu_item) const {
      Serial.print(menu_item.get_name());
  }
  
  void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
      String buffer;
  
      buffer = menu_item.get_name();
      buffer += menu_item.has_focus() ? '<' : '=';
      buffer += menu_item.get_formatted_value();
  
      if (menu_item.has_focus())
          buffer += '>';
  
      Serial.print(buffer);
  }
  
  void render_menu(Menu const& menu) const {
      Serial.print(menu.get_name());
  }
};
