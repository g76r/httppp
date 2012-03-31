#ifndef TCPPACKETMODEL_H
#define TCPPACKETMODEL_H

#include <QAbstractItemModel>
#include "pcaptcppacket.h"
#include "pcaptcpconversation.h"

class TcpPacketModel : public QAbstractItemModel
{
  Q_OBJECT
private:
  class TreeItem {
  public:
    TreeItem *_parent;
    PcapTcpPacket _packet;
    PcapTcpConversation _conversation;
    QList<TreeItem *> _children;
    /** Means upstream for packets (if _conversation.isNull()) and finished
      * for conversations.
      */
    bool _upstreamFinished;

    TreeItem(TreeItem *parent, PcapTcpPacket packet,
             PcapTcpConversation conversation, bool upstream)
      : _parent(parent), _packet(packet), _conversation(conversation),
        _upstreamFinished(upstream) { }
    ~TreeItem() {
      qDeleteAll(_children);
    }
    int row() /*const*/ {
      return _parent ? _parent->_children.indexOf(this) : 0;
    }

  private:
    Q_DISABLE_COPY(TreeItem)
  };

  TreeItem* _root;

public:
  explicit TcpPacketModel(QObject *parent = 0);
  ~TcpPacketModel();
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  //Qt::ItemFlags flags(const QModelIndex & index) const;
  QModelIndex index(PcapTcpConversation) const;
  /** @return packet pointed by index or null packet (isNull()) if index points
    * to a conversation
    */
  PcapTcpPacket packet(const QModelIndex &index) const;
  /** @return conversation pointed by index, or conversation to which packet
    * pointed by index belong to.
    */
  PcapTcpConversation conversation(const QModelIndex &index) const;

signals:
  
public slots:
  void addTcpUpstreamPacket(PcapTcpPacket packet,
                            PcapTcpConversation conversation);
  void addTcpDownstreamPacket(PcapTcpPacket packet,
                              PcapTcpConversation conversation);

private:
  void addPacket(PcapTcpConversation conversation, PcapTcpPacket packet,
                 bool upstream);
};

#endif // TCPPACKETMODEL_H
