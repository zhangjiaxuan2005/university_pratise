@Entry
@Component
struct RowAndColumn {
  build() {
    Column({ space: 2 }) {
      Row({ space: 1 }) {
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
      }
      .justifyContent(FlexAlign.Center)
      .width(200)
      .height(100)
      .border({
        width: 2,
        color: 'red',
        radius: 10,
        style: BorderStyle.Dashed
      })

      Column({ space: 1 }) {
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
        Row().width(20).height(20).border({ width: 2 })
      }
      .justifyContent(FlexAlign.Center)
      .width(200)
      .height(300)
      .border({
        width: 2,
        color: 'red',
        radius: 10,
        style: BorderStyle.Dashed
      })

      Row({ space: 3 }) {
        Row().width(30).height(30).border({ width: 2 })
        Row().width(30).height(30).border({ width: 2 })
        Row().width(30).height(30).border({ width: 2 })
        Row().width(30).height(30).border({ width: 2 })
        Row().width(30).height(30).border({ width: 2 })
        Row().width(30).height(30).border({ width: 2 })
      }
      .justifyContent(FlexAlign.SpaceBetween)//两端对齐
      .width(200)
      .height(100)
      .border({
        width: 2,
        color: 'red',
        radius: 10,
        style: BorderStyle.Dashed
      })
    }
  }
}