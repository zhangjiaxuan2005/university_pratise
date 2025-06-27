@Component
export struct MtFooter {
  build() {
    Row() {
      Text("我是Footer")
        .fontSize(24)
    }
    .justifyContent(FlexAlign.Center)
    .width("100%")
    .height(54)
    .backgroundColor(Color.Yellow)
  }
}